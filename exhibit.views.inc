<?php
// $Id$

//////////////////////////////////////////////////////////////////////////////
// Views API hooks

/**
 * Implementation of hook_views_plugins().
 */
function exhibit_views_plugins() {
  return array(
    // Style plugins
    'style' => array(
      'exhibit_json' => array(
        'title' => t('Exhibit JSON'),
        'help' => t('Generates an Exhibit JSON feed from a view.'),
        'handler' => 'views_plugin_style_exhibit_json',
        'path' => drupal_get_path('module', 'exhibit') .'/contrib/exhibit_views',
        'uses row plugin' => TRUE,
        'uses fields' => TRUE,
        'uses options' => TRUE,
        'type' => 'feed',
        'help topic' => 'style-exhibit-json',
      ),
    ),

    // Row plugins
    'row' => array(
      'node_exhibit' => array(
        'title' => t('Fields (Exhibit)'),
        'help' => t('Outputs the fields as an Exhibit JSON object.'),
        'handler' => 'views_plugin_row_fields_exhibit',
        'path' => drupal_get_path('module', 'exhibit') .'/contrib/exhibit_views',
        'uses fields' => TRUE,
        'uses options' => FALSE,
        'type' => 'feed',
        'help topic' => 'style-node-exhibit',
      ),
    ),
  );
}
