<?php
/**
 * @file
 * Admin forms to manage module
 *
 */

/**
 * Menu callback. Display removed pages.
 *
 * @param $url_path URL path to be passed on to backdrop_get_form()
 * for use as the default value of the URL path form field.
 */
function gone_410($url_path = '') {
  $items_per_page = config_get('gone_410.settings', 'items_per_page');
  $rows   = array();
  $header = array(
    array('data' => t('URL Path'), 'field' => 'url_path'),
    array('data' => t('Inserted'), 'field' => 'inserted', 'sort' => 'desc'),
    array('data' => t('Return 404?'), 'field' => 'return_404'),
    array('data' => t('Count of requests'), 'field' => 'count'),
    array('data' => t('Last request'), 'field' => 'last_request'),
    array('data' => t('Operations')),
  );

  $select = db_select('gone_410', 'g')
    ->extend('PagerDefault')
    ->extend('TableSort');

  $select->fields('g', array('url_path_id', 'url_path', 'inserted', 'return_404', 'count', 'last_request'))
    ->limit($items_per_page)
    ->orderByHeader($header);

  $results = $select->execute();
  foreach ($results as $result) {
    $rows[] = array(
      !empty($result->url_path) ? check_plain($result->url_path) : 'bad record, remove it',
      !empty($result->inserted) ? format_date($result->inserted, 'short') : t('N/A'),
      $result->return_404 ? t('Yes') : t('No'),
      $result->count,
      $result->last_request > 0 ? format_date($result->last_request, 'short') : 'N/A',
      l(t('remove'), "admin/config/urls/gone-410/remove/$result->url_path_id", array('attributes' => array('title' => 'Remove this URL path from the list'))),
    );
  }

  $build['gone_410_form'] = backdrop_get_form('gone_410_form', $url_path);
  $output  = backdrop_render($build);
  $output .= theme('table', array('header' => $header, 'rows' => $rows));
  $output .= theme('pager');

  return $output;
}

/**
 * Define the form for insert URL path.
 *
 */
function gone_410_form($form, &$form_state, $url_path) {
  $config = config('gone_410.settings');

  $form['url_path'] = array(
    '#title' => t('Add URL path to the list'),
    '#type' => 'textfield',
    '#description' => t('Add a new URL path to the list of unwanted requests, for example: some-expired-page.html') . '<br>'
      . t('Query parameters and anchors will be ignored and will be removed.'),
    '#size' => 60,
    '#maxlength' => 255,
    '#default_value' => $url_path,
  );

  $form['return_404'] = array(
    '#title' => t('Return 404 instead of 410'),
    '#type' => 'checkbox',
    '#default_value' => 0,
    '#description' => t('Select this if you want to return 404 instead of 410 for this specific URL path.'),
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add'),
  );

  $form['#submit'][] = 'gone_410_form_submit';
  $form['#validate'][] = 'gone_410_form_validate';

  return $form;
}

function gone_410_form_validate($form, &$form_state) {
  // cleanup entered path
  $raw_url_path = trim($form_state['values']['url_path'], ' /');
  $url_path = strtok($raw_url_path, '?#');

  if (empty($url_path)) {
    form_set_error('url_path', t('This field cannot be blank. Characters like / or ? or # are ignored if they are the only ones present.'));
  }

  if (db_query("SELECT * FROM {gone_410} WHERE url_path = :url_path", array(':url_path' => $url_path))->fetchField()) {
    form_set_error('url_path', t('This URL path address is already listed.'));
  }
  // set cleaned path value for submit handler
  form_set_value($form['url_path'], $url_path, $form_state);
}

function gone_410_form_submit($form, &$form_state) {
  $url_path = $form_state['values']['url_path'];
  $return_404 = $form_state['values']['return_404'];
  $time = time();

  db_insert('gone_410')
    ->fields(array('url_path' => $url_path, 'inserted' => $time, 'return_404' => $return_404))
    ->execute();

  backdrop_set_message(t('The URL path %url_path was added to the list.', array('%url_path' => $url_path)));
  watchdog('gone_410', 'URL path %url_path was added to the list.', array('%url_path' => $url_path));
  $form_state['redirect'] = 'admin/config/urls/gone-410';
}

/**
 * URL path remove from list confirm page.
 *
 */
function gone_410_remove_from_list($form, &$form_state, $url_path_id) {
  $form['url_path_to_remove'] = array(
    '#type' => 'value',
    '#value' => $url_path_id,
  );

  $url_path = db_select('gone_410', 'g')
    ->fields('g', array('url_path'))
    ->condition('g.url_path_id', $url_path_id)
    ->execute()
    ->fetchField();

  $form['url_path_removed'] = array(
    '#type' => 'value',
    '#value' => $url_path,
  );

  return confirm_form($form, t('Are you sure you want to remove %url_path from the list?',
    array('%url_path' => $url_path)), 'admin/config/urls/gone-410',
  t('This action cannot be undone.'), t('Remove'), t('Cancel'));
}

/**
 * URL path remove from list form submissions.
 *
 */
function gone_410_remove_from_list_submit($form, &$form_state) {
  $url_path_id = $form_state['values']['url_path_to_remove'];
  $url_path = $form_state['values']['url_path_removed'];

  db_delete('gone_410')
    ->condition('url_path_id', $url_path_id)
    ->execute();

  watchdog('gone_410', 'URL path %url_path was removed from the list.', array('%url_path' => $url_path));
  backdrop_set_message(t('URL path %url_path was removed from the list.', array('%url_path' => $url_path)));
  $form_state['redirect'] = 'admin/config/urls/gone-410';
}
