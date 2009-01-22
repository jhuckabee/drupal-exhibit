<?php
// $Id$

/**
 * Get search results
 */
function exhibit_apachesolr_get_search($keys, $options = array()) {
  $types  = array('search' => array('label' => t('Result'), 'pluralLabel' => t('Results')));
  $fields = array(
    'score'      => array('valueType' => 'number'),
    'author'     => array('valueType' => 'item'),
    'created'    => array('valueType' => 'date'),
    'type'       => array('valueType' => 'item'),
    'url'        => array('valueType' => 'url'),
  );
  $results = apachesolr_search_search('search', $keys);
  foreach ($results as $result) {
    $items[] = exhibit_compact_item(array(
      'type'        => 'search',
      'id'          => $result['link'],
      'label'       => $result['title'],
      'author'      => $result['user'],
      'created'     => gmstrftime(EXHIBIT_DATE_FORMAT, $result['date']),
      'score'       => $result['score'],
      'snippet'     => $result['snippet'],
      'node_type'   => $result['type'],
      'url'         => $result['link'],
    ));
  }

  $filename = drupal_get_path('module', 'exhibit') .'/exhibit.pages.inc';
  require_once $filename;
  exhibit_output('application/json', drupal_to_js(exhibit_json($items, $types, $fields)));
}