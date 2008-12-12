<?php
// $Id$

//////////////////////////////////////////////////////////////////////////////
// Exhibit API functions

function exhibit_admin_feed_types() {
  return array(
    'application/json'         => t('Exhibit JSON'),
    'application/rdf+xml'      => t('RDF/XML'),
    'application/n3'           => t('N3'),
    'application/vnd.ms-excel' => t('Excel'),
    'text/plain'               => t('Tab-separated values'),
    'application/x-bibtex'     => t('Bibtex'),
    'application/vnd.google-spreadsheet' => t('Google Spreadsheet'),
  );
}

//////////////////////////////////////////////////////////////////////////////
// Exhibit settings

/**
 * Form builder: displays the module configuration screen.
 *
 * @ingroup forms
 * @see system_settings_form()
 */
function exhibit_admin_settings() {
  $form = array();

  // Exhibit API
  $form['api'] = array('#type' => 'fieldset', '#title' => t('Exhibit API'), '#collapsible' => TRUE, '#collapsed' => FALSE);
  $form['api']['exhibit_js_url'] = array('#type' => 'textfield', '#title' => t('Exhibit JS API'), '#default_value' => EXHIBIT_JS_URL, '#description' => t('Specify the location from where the Exhibit 2.0 JavaScript <acronym title="Application Programming Interface">API</acronym> will be loaded. By default, it is loaded directly from <a href="http://simile.mit.edu/" target="_blank">SIMILE\'s</a> servers, ensuring the use of the very latest version. However, should you require it, there are instructions available on how to <a href="http://simile.mit.edu/wiki/Exhibit/2.0/Running_Exhibit_Yourself" target="_blank">run Exhibit locally</a>.'));

  $extensions = array();
  foreach (exhibit_exhibit_views() as $id => $view) {
    if (isset($view->href)) {
      $extensions[$id] = $view['title'];
    }
  }

  // View settings
  $form['extensions'] = array('#type' => 'fieldset', '#title' => t('Exhibit extensions'), '#collapsible' => TRUE, '#collapsed' => TRUE);
  $form['extensions']['exhibit_extensions'] = array('#type' => 'checkboxes', '#title' => t('Enable extensions'), '#options' => $extensions, '#default_value' => variable_get('exhibit_extensions', array()), '#description' => t(''));
  $form['extensions']['exhibit_gmap_key'] = array('#type' => 'textfield', '#title' => t('Google Maps API key'), '#default_value' => variable_get('exhibit_gmap_key', ''), '#description' => t(''));

  return system_settings_form($form);
}

//////////////////////////////////////////////////////////////////////////////
// Exhibit feed administration

/**
 * Menu callback: displays the data feeds management page.
 */
function exhibit_admin_feeds() {
  $head = array(t('Title'), t('Type'), t('URL'), array('data' => t('Operations'), 'colspan' => 2));
  $rows = array();

  $types = exhibit_admin_feed_types();
  foreach (exhibit_get_feeds() as $feed) {
    $url_parts = (exhibit_is_link_internal($feed->url) ? parse_url($feed->url) : NULL);

    $rows[] = array(
      is_numeric($feed->fid) ? check_plain($feed->title) : $feed->title,
      check_plain(isset($types[$feed->type]) ? $types[$feed->type] : $feed->type),
      (!is_null($url_parts) ?
        l(truncate_utf8($feed->url, 56, TRUE, TRUE), $url_parts['path'], array('query' => $url_parts['query'])) :
        l(truncate_utf8($feed->url, 56, TRUE, TRUE), $feed->url)),
      is_numeric($feed->fid) ? l(t('edit'), 'admin/content/exhibit/edit/' . $feed->fid) : '',
      is_numeric($feed->fid) ? l(t('delete'), 'admin/content/exhibit/delete/' . $feed->fid) : '',
    );
  }

  if (empty($rows)) {
    $rows[] = array(array('data' => t('No data feeds defined.'), 'colspan' => '5'));
  }
  return theme('table', $head, $rows);
}

/**
 * Form builder: generates a form for adding/editing data feeds.
 *
 * @ingroup forms
 * @see exhibit_admin_feed_form_validate()
 * @see exhibit_admin_feed_form_submit()
 */
function exhibit_admin_feed_form(&$form_state, $edit = array('title' => '', 'type' => 'application/json', 'url' => '')) {
  $form = array();
  $form['fid'] = array('#type' => 'hidden', '#value' => !empty($edit['fid']) ? $edit['fid'] : '');
  $form['module'] = array('#type' => 'hidden', '#value' => '');

  $form['title'] = array('#type' => 'textfield', '#title' => t('Title'), '#default_value' => $edit['title'], '#maxlength' => 64, '#description' => t('The human-readable name of this data feed. This text will be displayed as part of the list on the data feed management page. This name must begin with a capital letter and contain only letters, numbers, and spaces. This name must be unique.'), '#required' => TRUE);
  $form['url'] = array('#type' => 'textfield', '#title' => t('URL'), '#default_value' => $edit['url'], '#maxlength' => 255, '#description' => t('The URL of this data feed, given either as a relative internal path or a full absolute URL.'), '#required' => TRUE);

  $form['type'] = array('#type' => 'select', '#title' => t('Type'), '#default_value' => $edit['type'], '#options' => exhibit_admin_feed_types(), '#description' => t('The content type of this data feed. Only the JSON content type is natively supported; any other content types will be automatically converted when loaded.'), '#required' => TRUE);

  $form['submit'] = array('#type' => 'submit', '#value' => empty($edit['fid']) ? t('Create new feed') : t('Update feed settings'));
  return $form;
}

function exhibit_admin_feed_form_validate($form, &$form_state) {
  extract($form_state['values'], EXTR_SKIP | EXTR_REFS);

  if (!valid_url($url, FALSE) && !valid_url($url, TRUE)) {
    form_set_error('url', t('%url is not a valid URL.', array('%url' => $url)));
  }

  // TODO: check for duplicate titles and URLs.
}

function exhibit_admin_feed_form_submit($form, &$form_state) {
  extract($form_state['values'], EXTR_SKIP | EXTR_REFS);

  if (empty($fid)) {
    db_query("INSERT INTO {exhibit_feeds} (fid, module, title, type, url) VALUES (NULL, '%s', '%s', '%s', '%s')", $module, $title, $type, $url);
    watchdog('exhibit', 'Data feed %title created.', array('%title' => $title), WATCHDOG_NOTICE, l(t('view'), 'admin/content/exhibit'));
    drupal_set_message(t('The data feed %title has been created.', array('%title' => $title)));
  }
  else {
    db_query("UPDATE {exhibit_feeds} SET title = '%s', type = '%s', url = '%s' WHERE fid = %d", $title, $type, $url, $fid);
    watchdog('exhibit', 'Data feed %title updated.', array('%title' => $title), WATCHDOG_NOTICE, l(t('view'), 'admin/content/exhibit'));
    drupal_set_message(t('The data feed %title has been updated.', array('%title' => $title)));
  }

  $form_state['redirect'] = 'admin/content/exhibit';
}

function exhibit_admin_feed_delete($form_state, $feed) {
  $form['fid'] = array('#type' => 'value', '#value' => $feed['fid']);
  $form['title'] = array('#type' => 'value', '#value' => $feed['title']);

  return confirm_form($form,
    t('Are you sure you want to delete the data feed %title?', array('%title' => $feed['title'])),
    isset($_GET['destination']) ? $_GET['destination'] : 'admin/content/exhibit',
    t('This action cannot be undone.'));
}

function exhibit_admin_feed_delete_submit($form, &$form_state) {
  extract($form_state['values'], EXTR_SKIP | EXTR_REFS);

  if ($form_state['values']['confirm']) {
    db_query("DELETE FROM {exhibit_feeds} WHERE fid = %d", $fid);
    watchdog('exhibit', 'Data feed %title deleted.', array('%title' => $title));
    drupal_set_message(t('The data feed %title has been deleted.', array('%title' => $title)));
  }

  $form_state['redirect'] = 'admin/content/exhibit';
}
