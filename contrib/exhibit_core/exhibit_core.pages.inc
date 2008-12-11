<?php
// $Id: exhibit_core.pages.inc,v 1.4 2008/07/17 11:51:07 arto Exp $

//////////////////////////////////////////////////////////////////////////////
// Exhibit data feeds

function exhibit_core_get_users($role = NULL, $options = array()) {
  $types  = array('user' => array('label' => t('User'), 'pluralLabel' => t('Users')));
  $fields = array(
    'created'    => array('valueType' => 'date'),
    'access'     => array('valueType' => 'date'),
    'login'      => array('valueType' => 'date'),
    'url'        => array('valueType' => 'url'),
  );

  // Make sure the current user is allowed to see user profile information.
  if (!user_access('access user profiles'))
    return exhibit_json(array(), $types, $fields);

  // Retrieve users, filtering by an optional user role limitation, if specified
  $result = $role && $role != 'authenticated user' ?
    db_query("SELECT u.* FROM {users} u LEFT JOIN {users_roles} ur ON u.uid = ur.uid LEFT JOIN {role} r ON r.rid = ur.rid WHERE r.name = '%s'", $role) :
    db_query('SELECT u.uid FROM {users} u WHERE u.status = 1 ORDER BY u.name');

  $items = array();
  while ($user = db_fetch_object($result)) {
    $user = user_load($user->uid);

    $item = array(
      'type'        => 'user',
      'id'          => 'user/' . $user->uid,
      'label'       => $user->name, // TODO: use profile fields if possible
      'name'        => $user->name,
      'mail'        => $user->mail,
      'created'     => gmstrftime(EXHIBIT_DATE_FORMAT, $user->created),
      'access'      => gmstrftime(EXHIBIT_DATE_FORMAT, $user->access),
      'login'       => gmstrftime(EXHIBIT_DATE_FORMAT, $user->login),
      'language'    => $user->language, // TODO: should probably be language label, not ID
      'roles'       => array_values($user->roles),
      'url'         => url('user/' . $user->uid, array('absolute' => TRUE)),
    );

    // Add profile fields if the module is enabled
    if (module_exists('profile')) {
      $profile_result = db_query('SELECT f.name name, v.value value FROM {profile_values} v, {profile_fields} f WHERE v.fid = f.fid AND v.uid = %d AND f.visibility = 3', $user->uid);
      while ($row = db_fetch_array($profile_result)) {
        if(!empty($row['name']) && !is_null($row['value'])) {
          $item[$row['name']] = $row['value'];
        }
      }
    }

    $items[] = exhibit_compact_item($item);
  }

  return exhibit_json($items, $types, $fields);
}

function exhibit_core_get_terms($vocabulary, $options = array()) {
  $types  = array('taxonomy/term' => array('label' => t('Term'), 'pluralLabel' => t('Terms')));
  $fields = array(
    //'weight'     => array('valueType' => 'number'),
    'url'        => array('valueType' => 'url'),
  );

  $result = db_query('SELECT t.* FROM {term_data} t WHERE t.vid = %d ORDER BY t.name', $vocabulary->vid);
  $items = array();
  while ($term = db_fetch_object($result)) {
    // TODO: take hierarchical term relationships into account.

    $items[] = exhibit_compact_item(array(
      'type'        => 'taxonomy/term',
      'id'          => 'taxonomy/term/' . $term->tid,
      'label'       => $term->name,
      'description' => $term->description,
      //'weight'      => (int)$term->weight, // TODO: needed for anything?
      'url'         => url('taxonomy/term/' . $term->tid, array('absolute' => TRUE)),
    ));
  }

  return exhibit_json($items, $types, $fields);
}

function exhibit_core_get_nodes($type = NULL, $options = array()) {
  $types  = array('node' => array('label' => t('Node'), 'pluralLabel' => t('Nodes')));
  $fields = array(
    'author'     => array('valueType' => 'item'),
    'created'    => array('valueType' => 'date'),
    'changed'    => array('valueType' => 'date'),
    'url'        => array('valueType' => 'url'),
  );

  $result = !$type ?
    db_query('SELECT n.nid FROM {node} n WHERE n.status = 1 ORDER BY n.nid') :
    db_query("SELECT n.nid FROM {node} n WHERE n.status = 1 AND n.type = '%s' ORDER BY n.nid", $type);
  $items = array();
  while ($node = db_fetch_object($result)) {
    $node = node_load($node->nid);

    // Make sure that the current user actually should be seeing to this node.
    // TODO: double-check interactions with data feed caching.
    if (!node_access('view', $node))
      continue;

    // If this node type has not yet been seen, amend the Exhibit schema.
    // TODO: there's a problem here in that we don't have any means of
    // obtaining plural labels for content types; just appending an 's' will
    // not get us all that far even in English. Good suggestions welcome.
    if (!isset($types[$node->type])) {
      $type_label = node_get_types('name', $node->type);
      $type_label = $type_label ? $type_label : ucwords($node->type);
      $types[$node->type] = array('label' => $type_label);
    }

    $items[] = exhibit_compact_item(array(
      'type'        => $node->type,
      'id'          => 'node/' . $node->nid,
      'label'       => $node->title,
      'author'      => 'user/' . $node->uid,
      'created'     => gmstrftime(EXHIBIT_DATE_FORMAT, $node->created),
      'changed'     => gmstrftime(EXHIBIT_DATE_FORMAT, $node->changed),
      'body'        => $node->teaser,
      'url'         => url('node/' . $node->nid, array('absolute' => TRUE)),
      'terms'       => array_values(array_map('_exhibit_core_get_term_name', taxonomy_node_get_terms($node))),
    ));
  }

  return exhibit_json($items, $types, $fields);
}

function _exhibit_core_get_term_name($term) {
  return $term->name;
}
