<?php
// $Id$

//////////////////////////////////////////////////////////////////////////////
// Views API plugins

/**
 * Views style plugin that outputs an Exhibit JSON feed.
 *
 * @see [TODO: link to Exhibit feed doc]
 * @ingroup views_style_plugins
 */
class views_plugin_style_exhibit_json extends views_plugin_style {
  function attach_to($display_id, $path, $title) {
    $display = $this->view->display[$display_id]->handler;
    $url_options = array();
    if (($input = $this->view->get_exposed_input())) {
      $url_options['query'] = $input;
    }

    $title = !empty($title) ? $title : t('Exhibit JSON');
    $url = url($this->view->get_url(NULL, $path), $url_options);
    if ($display->has_path()) {
      if (empty($this->preview)) {
        exhibit_add_link($url);
      }
    }
    else {
      if (empty($this->view->feed_icon)) {
        $this->view->feed_icon = '';
      }
      $this->view->feed_icon .= theme('feed_icon', $url, $title);
      exhibit_add_link($url);
    }
  }

  function option_definition() {
    $options = parent::option_definition();
    $options['type_label'] = array('default' => $this->view->base_table, 'translatable' => TRUE);
    $options['type_plural_label'] = array('default' => '', 'translatable' => TRUE);
    return $options;
  }

  function options_form(&$form, &$form_state) {
    $form['type_label'] = array(
      '#type' => 'textfield',
      '#title' => t('Type Label'),
      '#default_value' => $this->options['type_label'],
      '#description' => t('This is the label for the type of items in the feed.'),
    );
    $form['type_plural_label'] = array(
      '#type' => 'textfield',
      '#title' => t('Type Plural Label'),
      '#default_value' => $this->options['type_plural_label'],
      '#description' => t('The plural label for the type of items in the feed.'),
    );
  }

  function render() {
    $view_data = views_fetch_data($this->view->base_table);
    $base_field = $view_data['table']['base']['field'];

    if (empty($this->row_plugin)) {
      vpr('views_plugin_style_exhibit: Missing row plugin');
      return;
    }

    // Setup item types
    $types[$this->view->base_table] = array('label' => $this->options['type_label'],
                                      'pluralLabel' => $this->options['type_plural_label']);

    // Setup item properties
    $props = array();
    foreach ($this->view->field as $id => $field) {
      $props[$id] = array('label' => $field->options['label']);
      // Set format for default node fields
      if ($field->options['date_format']) {
        $props[$id]['valueType'] = 'date';
      }
      // Check CCK type fields
      if ($field->content_field && $type = $field->content_field['type']) {
        switch ($type) {
          case 'number_decimal':
          case 'number_float':
          case 'number_integer':
            $props[$id]['valueType'] = 'number';
            break;
          case 'date':
          case 'datetime':
          case 'datestamp':
            $props[$id]['valueType'] = 'date';
        }
      }
    }

    // Gather all rendered items
    $items = array();
    foreach ($this->view->result as $row) {
      $item = $this->row_plugin->render($row);
      // See if we already have values for this item, and merge if so
      if (!empty($items[$row->$base_field])) {
        // Add any new keys not already in the item
        $new_keys = array_diff_key($item, $items[$row->$base_field]);
        foreach($new_keys as $key => $value) {
          $items[$row->$base_field][$key] = $value;
        }
        // Check each value and convert differences into an array of values (think Taxonomy)
        foreach ($items[$row->$base_field] as $key => $value) {
          // If value is different, group it into an array
          if ($item[$key] && $item[$key] != $value) {
            if (is_array($value)) {
              if (is_array($item[$key])) {
               $items[$row->$base_field][$key] = array_merge($items[$row->$base_field][$key], $item[$key]);
              }
              else {
               $items[$row->$base_field][$key][] = $item[$key];
              }
            }
            else {
              $items[$row->$base_field][$key] = array_merge(array($value), is_array($item[$key]) ? $item[$key] : array($item[$key]));
            }
          }
        }
      }
      else {
        $items[$row->$base_field] = array_merge(array('label' => $row->$base_field, 'type' => $this->view->base_table), $item);
      }
    }

    module_load_include('inc', 'exhibit', 'exhibit.pages');
    exhibit_output('application/json', drupal_to_js(exhibit_json(array_values($items), $types, $props)));
    exit;
  }
}