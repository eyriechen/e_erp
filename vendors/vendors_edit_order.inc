<?php

function _order_delete($order) {
    return drupal_get_form('order_delete_form', $order);
}

function order_delete_form($form, &$form_state, $order) {
    $form_state['order'] = $order;
    $form['description'] = array(
        '#type' => 'item',
        '#title' => t('真的要删除吗？'),
    );
    $form['yes'] = array(
        '#type' => 'submit',
        '#value' => t('是'),
        '#submit' => array('order_delete_confirmed'),
    );
    $form['no'] = array(
        '#type' => 'submit',
        '#value' => t('否'),
        '#submit' => array('order_delete_not_confirmed', 2),
    );
    return $form;
}

function order_delete_confirmed($form, &$form_state) {
    $result = db_query('select vendor_id from {e_vendors_orders} where order_id= :order_id', array(':order_id' => $form_state['order']->order_id));
    $vendor_id = $result->fetch()->vendor_id;

    db_query('delete from e_vendors_orders where order_id= :order_id', array(':order_id' => $form_state['order']->order_id));
    db_query('delete from e_vendors_order_products where order_id= :order_id', array(':order_id' => $form_state['order']->order_id));
    drupal_set_message('删除成功');
    drupal_goto('vendors/' . $vendor_id);
}

function order_delete_not_confirmed($form, &$form_state) {
    drupal_goto('vendors/orders/' . $form_state['order']->order_id);
}