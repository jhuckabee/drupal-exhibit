<?php
// $Id: exhibit.pages.inc,v 1.2 2008/07/17 11:51:06 arto Exp $

//////////////////////////////////////////////////////////////////////////////
// Menu callbacks

function exhibit_output_convert($url, $callback, $input_format = NULL, $output_format = 'application/json') {
  if (!preg_match('!^exhibit/[\w]+/(.*)$!', $_GET['q'], $matches)) {
    return drupal_not_found();
  }

  $url = 'http://' . $matches[1];
  if (!valid_url($url, TRUE)) {
    return drupal_access_denied();
  }

  $response = drupal_http_request($url);
  if (!empty($response->error)) {
    // TODO
    return drupal_access_denied();
  }

  $output = $callback($response->data);

  exhibit_output($output_format, drupal_to_js($output));
}

function exhibit_output_node($node) {
  $function = 'exhibit_output_node_' . $node->type;
  exhibit_output('application/json', drupal_to_js(is_callable($function) ? call_user_func($function, $node) : array('items' => array())));
}

function exhibit_output_feed($feed) {
  exhibit_output($feed['type'], exhibit_get_feed_contents($feed['url']));
}

function exhibit_output($type, $body) {
  //$type = 'text/plain'; // DEBUG
  drupal_set_header('Content-Type: ' . $type . '; charset=utf-8');
  drupal_set_header('Content-Length: ' . strlen($body));

  if (EXHIBIT_FEED_ETAG) {
    $md5 = base64_encode(md5($body, TRUE));
    drupal_set_header('Content-MD5: ' . $md5);
    drupal_set_header('ETag: "' . $md5 . '"'); // strong entity tag
  }

  if (EXHIBIT_FEED_LIFETIME > 0) {
    drupal_set_header('Last-Modified: ' . gmdate('D, d M Y H:i:s') . ' GMT');
    drupal_set_header('Expires: ' . gmdate('D, d M Y H:i:s', time() + EXHIBIT_FEED_LIFETIME) . ' GMT');
    drupal_set_header('Cache-Control: max-age=' . EXHIBIT_FEED_LIFETIME . ', private, must-revalidate');
    drupal_set_header('Pragma: cache'); // need to override no-cache set by Drupal.
  }

  print $body;
}

/**
 * @see http://simile.mit.edu/wiki/Exhibit/Template/_history_.html
 */
function exhibit_output_history() {
  print '<html><body></body></html>';
}

//////////////////////////////////////////////////////////////////////////////

function exhibit_parse_tsv($input) {
  $output = array();

  $lines = explode("\n", $input);
  $fields = explode("\t", array_shift($lines));
  foreach ($lines as $line) {
    $item = array();
    foreach (explode("\t", $line) as $index => $value) {
      $item[$fields[$index]] = $value;
    }
    $output[] = $item;
  }

  return exhibit_json($output);
}
