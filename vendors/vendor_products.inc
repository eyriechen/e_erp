<?php

function products_table($vendor) {
    $header = array('产品名称', '单位', '价格');
    $result = db_query('select * from {e_vendors_products} where vendor_id = :vendor_id', array(':vendor_id' => $vendor->vendor_id));
    if ($result->rowCount() == 0)
        return '<br/>没有产品';
    $data = array();
    foreach ($result as $record) {
        $data[] = array(
            $record->product_name,
            $record->unit,
            $record->price,
        );
    }
    $output = theme('table', array('header' => $header,
        'rows' => $data));
    return $output;
}

function _vendor_products_render($vendor) {
    return l('编辑产品', 'vendors/' . $vendor->vendor_id . '/products/edit') . products_table($vendor);
}

function _vendor_products_edit($vendor) {
    return drupal_get_form('vendor_products_form', $vendor);
}

function vendor_products_form($form, &$form_state, $vendor) {
    $form['#tree'] = TRUE;
    $form_state['vendor'] = $vendor;

    $form['product_rows'] = array(
        '#type' => 'fieldset',
        '#title' => t('订单'),
        // Set up the wrapper so that AJAX will be able to replace the fieldset.
        '#prefix' => '<div id="product_rows">',
        '#suffix' => '</div>',
    );
    $form_state['first'] = FALSE;

    if (empty($form_state['row_num'])) {
        $form_state['first'] = TRUE;
        $result = db_query('select * from {e_vendors_products} where vendor_id = :vendor_id', array(':vendor_id' => $vendor->vendor_id));
        $form_state['row_num'] = $result->rowCount();
        if ($form_state['row_num'] == 0)
            $form_state['row_num'] = 1;
    }
    $i = 0;
    for (; $i < $form_state['row_num']; $i++) {
        if ($i == 0) {
            $form['product_rows'][$i]['name'] = array(
                '#type' => 'textfield',
                '#title' => t('品名'),
                '#prefix' => '<table id = "order_form"><thead><tr><th>品名</th><th>单位</th><th>单价</th></tr></thead><tr><td class = "form_name">',
                '#suffix' => '</td>',
                '#attributes' => array('size' => "35"),
            );
        } else {
            $form['product_rows'][$i]['name'] = array(
                '#type' => 'textfield',
                '#title' => t('品名'),
                '#prefix' => '<tr><td class="form_name">',
                '#suffix' => '</td>',
                '#attributes' => array('size' => "35"),
            );
        }
        $form['product_rows'][$i]['unit'] = array(
            '#type' => 'textfield',
            '#title' => t('单位'),
            '#prefix' => '<td class="form_unit">',
            '#suffix' => '</td>',
            '#attributes' => array('size' => "35"),
        );
        $form['product_rows'][$i]['price'] = array(
            '#type' => 'textfield',
            '#title' => t('单价'),
            '#prefix' => '<td class="form_price">',
            '#suffix' => '</td></tr>',
            '#attributes' => array('size' => "35"),
        );
    }


    $form['product_rows'][$i - 1]['price'] = array(
        '#type' => 'textfield',
        '#title' => t('单价'),
        '#prefix' => '<td class="form_price">',
        '#suffix' => '</td></tr></table>',
        '#attributes' => array('size' => "35"),
    );
    $form['product_rows']['add_row'] = array(
        '#type' => 'submit',
        '#value' => t('添加一行'),
        '#submit' => array('_add_product_add_row'),
        // See the examples in ajax_example.module for more details on the
        // properties of #ajax.
        '#ajax' => array(
            'callback' => 'ajax_add_product_add_row_callback',
            'wrapper' => 'product_rows',
            'event' => 'mousedown',
        ),
    );
    if ($form_state['row_num'] > 1) {
        $form['product_rows']['remove_name'] = array(
            '#type' => 'submit',
            '#value' => t('删除一行'),
            '#submit' => array('_add_product_remove_row'),
            '#ajax' => array(
                'callback' => 'ajax_add_product_add_row_callback',
                'wrapper' => 'product_rows',
            ),
        );
    }
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('提交'),
        '#validate' => array('_edit_product_form_submit_validate'),
    );

    if ($form_state['first']) {
        $result = db_query('select * from {e_vendors_products} where vendor_id = :vendor_id', array(':vendor_id' => $vendor->vendor_id));
        $i = 0;
        foreach ($result as $record) {
            $form['product_rows'][$i]['name']['#default_value'] = $record->product_name;
            $form['product_rows'][$i]['unit']['#default_value'] = $record->unit;
            $form['product_rows'][$i]['price']['#default_value'] = $record->price;
            $i++;
        }
    }
    return $form;
}

function _add_product_add_row($form, &$form_state) {
    $form_state['row_num']++;
    $form_state['rebuild'] = TRUE;
}

function ajax_add_product_add_row_callback($form, $form_state) {
    return $form['product_rows'];
}

function _add_product_remove_row($form, &$form_state) {
    if ($form_state['row_num'] > 1) {
        $form_state['row_num']--;
    }
    $form_state['rebuild'] = TRUE;
}

function vendor_products_form_submit($form, &$form_state) {
    $vendor = $form_state['vendor'];
    db_query('delete from {e_vendors_products} where vendor_id = :vendor_id', array(':vendor_id' => $vendor->vendor_id));
    for ($i = 0; $i < $form_state['row_num']; $i++) {
        db_query('insert into {e_vendors_products} (vendor_id,product_name,unit,price) values (:vendor_id,:product_name,:unit,:price)', array(
            ':vendor_id' => $vendor->vendor_id,
            ':product_name' => $form_state['values']['product_rows'][$i]['name'],
            ':unit' => $form_state['values']['product_rows'][$i]['unit'],
            ':price' => $form_state['values']['product_rows'][$i]['price'],
        ));
    }
    drupal_set_message('更新成功');
    drupal_goto('vendors/' . $vendor->vendor_id . '/products');
}
function _edit_product_form_submit_validate($form, &$form_state){
}