<?php
// $Id$

//////////////////////////////////////////////////////////////////////////////
// Views API plugins

/**
 * Views row plugin that invokes node_view() on each node and prepares its
 * body for inclusion as an RSS 1.0 item description in the feed.
 *
 * @see http://web.resource.org/rss/1.0/spec
 * @ingroup views_row_plugins
 */
class views_plugin_row_fields_exhibit extends views_plugin_row {

  function render($row) {
    $output = array();

    foreach ($this->view->field as $id => $field) {
      drupal_set_message('<pre>' . print_r($fiel, true) . '</pre>');
      // handle CCK multiple value fields
      if (array_key_exists('multiple', $field->options) && $field->aliases['vid'] == $field->field_alias) {
        $value = $this->_cck_multiple_render($field, $row);
      }
      // handle CCK date fields
      elseif ($field->content_field &&
              in_array($field->content_field['type'], array('date', 'datestamp', 'datetime'))) {
        $value = $this->_cck_date_render($field, $row);
      }
      elseif (array_key_exists('type', $field->options) &&
              array_key_exists('separator', $field->options) &&
              array_key_exists('empty', $field->options)) {
        $value = $this->_list_render($field, $row);
      }
      // handle all other fields
      else {
        $value = $field->render($row);
      }

      if (!empty($value)) {
        $output[$id] = $value;
      }
    }

    return $output;
  }

  function _cck_date_render($field, $values) {
    $alias = $field->field_alias;
    if ($field->position && $field->position == 2) {
      $alias = $alias. '2';
    }
    if (is_numeric($values->{$alias})) {
      return format_date($values->{$alias}, 'custom', "Y-m-d\TH:i:s");
    }
    else {
      return format_date(strtotime($values->{$alias}), 'custom', "Y-m-d\TH:i:s");
    }
  }

  function _list_render($field, $values) {
    $fid = $values->{$field->field_alias};
    if (!empty($field->items[$fid])) {
      return array_values($field->items[$fid]);
    }
    else if (!empty($field->options['empty'])) {
      return $field->options['empty'];
    }
  }

  function _cck_multiple_render($field, $values) {
    // If this is not a grouped field, use content_handler_field::render().
    if (!$field->defer_query || empty($field->field_values)) {
      return $field->render($values);
    }

    $content_field = $field->content_field;
    $options = $field->options;

    // $values will be used as a fake $node object;
    // we provide a build_mode for rendering.
    $values->build_mode = NODE_BUILD_NORMAL;

    // This needs to be set for the $this->render_link() to work. It would
    // have been set in the query, if we hadn't bypassed the normal query.
    // TODO there may be a better way to do this.
    $field->aliases['nid'] = 'nid';

    $count = 0;
    $items = array();
    $vid = $values->{$field->field_alias};
    if (isset($field->field_values[$vid])) {
      foreach ($field->field_values[$vid] as $item) {
        // 'Starting from n': Let the first n values pass.
        if (empty($options['multiple']['multiple_from']) || ($count >= $options['multiple']['multiple_from'])) {
          // 'Display m values'.
          if (empty($options['multiple']['multiple_number']) || (count($items) < $options['multiple']['multiple_number'])) {
            $value = content_format($content_field, $item, $options['format'], $values);
            if (!empty($value)) {
              $items[] = $field->render_link($value, $values);
            }
          }
          else {
            // We displayed the number of values that were asked, no need to render more.
            break;
          }
        }
        $count++;
      }
    }

    return $items;
  }

}

