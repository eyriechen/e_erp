<?php

function get_products($vendor) {
    $products = array();
    $result = db_query('select product_name from { e_vendors_products} where vendor_id = :vendor_id', array(':vendor_id' => $vendor->vendor_id));
    foreach ($result as $record) {
        $products[$record->product_name] = $record->product_name;
    }
    return $products;
}

function _add_order($vendor) {
    return drupal_get_form('_add_order_form', $vendor);
}

function _edit_order($order) {
    $vendor = vendor_load($order->vendor_id);
    return drupal_get_form('_add_order_form', $vendor, $order);
}

function _add_order_form($form, &$form_state, $vendor, $order = null) {
    $form['#tree'] = TRUE;
    $form_state['vendor'] = $vendor;
    $form_state['order'] = $order;

    $form['date'] = array(
        '#type' => 'textfield',
        '#title' => t('日期'),
        '#attributes' => array('size' => "20", 'id' => 'datepicker'),
        '#default_value' => date('Y-m-d'),
    );

    $form['order_rows'] = array(
        '#type' => 'fieldset',
        '#title' => t('订单'),
        // Set up the wrapper so that AJAX will be able to replace the fieldset.
        '#prefix' => '<div id="order_rows">',
        '#suffix' => '</div>',
    );
    $form_state['first'] = FALSE;
    if (empty($form_state['row_num'])) {
        $form_state['first'] = TRUE;
        if ($order != null) {
            $result = db_query('select * from {e_vendors_order_products} where order_id = :order_id', array(':order_id' => $order->order_id));
            $form_state['row_num'] = $result->rowCount();
            if ($form_state['row_num'] == 0)
                $form_state['row_num'] = 1;
        } else {
            $form_state['row_num'] = 1;
        }
    }
    $products = get_products($vendor);
    $i = 0;
    for (; $i < $form_state['row_num']; $i++) {
        if ($i == 0) {
            $form['order_rows'][$i]['name'] = array(
                '#type' => 'select',
                '#title' => t('品名'),
                '#options' => $products,
                '#prefix' => '<table id = "order_form"><thead><tr><th>品名</th><th>规格</th><th>单位</th><th>数量</th><th>单价</th><th>合计</th><th>交货日期</th><th>交货地点</th></tr></thead><tr><td class = "form_name">',
                '#suffix' => '</td>',
            );
        } else {
            $form['order_rows'][$i]['name'] = array(
                '#type' => 'select',
                '#title' => t('品名'),
                '#options' => $products,
                '#prefix' => '<tr><td class="form_name">',
                '#suffix' => '</td>',
            );
        }
        $form['order_rows'][$i]['size'] = array(
            '#type' => 'textfield',
            '#title' => t('规格'),
            '#prefix' => '<td class="form_size">',
            '#suffix' => '</td>',
            '#attributes' => array('size' => "1"),
        );
        $form['order_rows'][$i]['unit'] = array(
            '#type' => 'textfield',
            '#title' => t('单位'),
            '#prefix' => '<td class="form_unit">',
            '#suffix' => '</td>',
            '#attributes' => array('size' => "1"),
            '#default_value' => 'T',
        );
        $form['order_rows'][$i]['amount'] = array(
            '#type' => 'textfield',
            '#title' => t('数量'),
            '#prefix' => '<td class="form_amount">',
            '#suffix' => '</td>',
            '#attributes' => array('size' => "5"),
        );
        $form['order_rows'][$i]['price'] = array(
            '#type' => 'textfield',
            '#title' => t('单价'),
            '#prefix' => '<td class="form_price">',
            '#suffix' => '</td>',
            '#attributes' => array('size' => "6"),
        );
        $form['order_rows'][$i]['sum'] = array(
            '#type' => 'textfield',
            '#title' => t('合计'),
            '#prefix' => '<td class="form_sum">',
            '#suffix' => '</td>',
            '#attributes' => array('size' => "6"),
        );
        $form['order_rows'][$i]['delivery_date'] = array(
            '#type' => 'textfield',
            '#title' => t('交货日期'),
            '#prefix' => '<td class="form_delivery_date">',
            '#suffix' => '</td>',
            '#attributes' => array('size' => "5"),
            '#default_value' => '尽快',
        );
        $form['order_rows'][$i]['place'] = array(
            '#type' => 'textfield',
            '#title' => t('交货地点'),
            '#prefix' => '<td class="form_place">',
            '#suffix' => '</td></tr>',
            '#attributes' => array('size' => "5"),
            '#default_value' => '本公司',
        );
    }


    $form['order_rows'][$i - 1]['place'] = array(
        '#type' => 'textfield',
        '#title' => t('交货地点'),
        '#prefix' => '<td class="form_place">',
        '#suffix' => '</td></tr></table>',
        '#attributes' => array('size' => "5"),
        '#default_value' => '本公司',
    );
    $form['order_rows']['add_row'] = array(
        '#type' => 'submit',
        '#value' => t('添加一行'),
        '#submit' => array('_add_order_add_row'),
        // See the examples in ajax_example.module for more details on the
        // properties of #ajax.
        '#ajax' => array(
            'callback' => 'ajax_add_order_add_row_callback',
            'wrapper' => 'order_rows',
            'event' => 'mousedown',
        ),
    );
    if ($form_state['row_num'] > 1) {
        $form['order_rows']['remove_name'] = array(
            '#type' => 'submit',
            '#value' => t('删除一行'),
            '#submit' => array('_add_order_remove_row'),
            '#ajax' => array(
                'callback' => 'ajax_add_order_add_row_callback',
                'wrapper' => 'order_rows',
            ),
        );
    }
    $form['remarks'] = array(
        '#type' => 'textfield',
        '#title' => t('订单备注'),
        '#default_value' => '要求磷化丝。表明应光滑',
    );
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('提交'),
        '#validate' => array('_add_order_form_submit_validate'),
    );

    if ($form_state['first']) {
        if ($order != null) {
            $result = db_query('select * from {e_vendors_order_products} where order_id = :order_id', array(':order_id' => $order->order_id));
            $i = 0;
            foreach ($result as $record) {
                $form['order_rows'][$i]['product_name']['#default_value'] = $record->product_name;
                $form['order_rows'][$i]['size']['#default_value'] = $record->size;
                $form['order_rows'][$i]['unit']['#default_value'] = $record->unit;
                $form['order_rows'][$i]['amount']['#default_value'] = $record->amount;
                $form['order_rows'][$i]['price']['#default_value'] = $record->price;
                $form['order_rows'][$i]['sum']['#default_value'] = $record->sum;
                $form['order_rows'][$i]['delivery_date']['#default_value'] = $record->delivery_date;
                $form['order_rows'][$i]['place']['#default_value'] = $record->place;
                $i++;
            }
            $form['date']['#default_value'] = $order->date;
            $form['remarks']['#default_value'] = $order->remarks;
        }
    }
    return $form;
}

function _add_order_add_row($form, &$form_state) {
    $form_state['row_num']++;
    $form_state['rebuild'] = TRUE;
}

function ajax_add_order_add_row_callback($form, $form_state) {
    return $form['order_rows'];
}

function _add_order_remove_row($form, &$form_state) {
    if ($form_state['row_num'] > 1) {
        $form_state['row_num']--;
    }
    $form_state['rebuild'] = TRUE;
}

function _add_order_form_submit($form, &$form_state) {
    if ($form_state['order'] != null) {
        db_query('delete from e_vendors_order_products where order_id= :order_id', array(':order_id' => $form_state['order']->order_id));
        db_query('update e_vendors_orders set vendor_id=:vendor_id,date=:date,remarks=:remarks where order_id = :order_id', array(
            ':vendor_id' => $form_state['vendor']->vendor_id,
            ':date' => $form_state['values']['date'],
            ':remarks' => $form_state['values']['remarks'],
            'order_id' => $form_state['order']->order_id,
                )
        );
        $order_id = $form_state['order']->order_id;
    } else {
        db_query('insert into e_vendors_orders (vendor_id,date,remarks) values (:vendor_id,:date,:remarks)', array(
            ':vendor_id' => $form_state['vendor']->vendor_id,
            ':date' => $form_state['values']['date'],
            ':remarks' => $form_state['values']['remarks'],
                )
        );
        $result = db_query('select last_insert_id() as lid');
        $order_id = $result->fetch()->lid;
    }


    for ($i = 0; $i < $form_state['row_num']; $i++) {
        db_query('insert into e_vendors_order_products (order_id,product_name,size,unit,amount,price,sum,delivery_date,place) values (:order_id,:product_name,:size,:unit,:amount,:price,:sum,:delivery_date,:place)', array(
            ':order_id' => $order_id,
            ':product_name' => $form_state['values']['order_rows'][$i]['name'],
            ':size' => $form_state['values']['order_rows'][$i]['size'],
            ':unit' => $form_state['values']['order_rows'][$i]['unit'],
            ':amount' => $form_state['values']['order_rows'][$i]['amount'],
            ':price' => $form_state['values']['order_rows'][$i]['price'],
            ':sum' => $form_state['values']['order_rows'][$i]['sum'],
            ':delivery_date' => $form_state['values']['order_rows'][$i]['delivery_date'],
            ':place' => $form_state['values']['order_rows'][$i]['place'],
                )
        );
    }
    if ($form_state['order'] != null) {
        drupal_set_message('更新成功');
        drupal_goto('vendors/orders/' . $form_state['order']->order_id);
    } else {
        drupal_set_message('添加成功');
        drupal_goto('vendors/' . $form_state['vendor']->vendor_id);
    }
}

function _add_order_form_submit_validate($form, &$form_state) {
    if ($form_state['values']['date'] == '') {
        form_set_error('date', '日期不能为空');
    }
    for ($i = 0; $i < $form_state['row_num']; $i++) {
        if ($form_state['values']['order_rows'][$i]['size'] == '')
            form_set_error('order_rows][' . $i . '][size', '规格不能为空');
        if ($form_state['values']['order_rows'][$i]['unit'] == '')
            form_set_error('order_rows][' . $i . '][unit', '单位不能为空');
        if ($form_state['values']['order_rows'][$i]['amount'] == '')
            form_set_error('order_rows][' . $i . '][amount', '数量不能为空');
        if ($form_state['values']['order_rows'][$i]['price'] == '')
            form_set_error('order_rows][' . $i . '][price', '价格不能为空');
        if ($form_state['values']['order_rows'][$i]['sum'] == '')
            form_set_error('order_rows][' . $i . '][sum', '合计不能为空');
        if (!is_numeric($form_state['values']['order_rows'][$i]['sum']))
            form_set_error('order_rows][' . $i . '][sum', '合计不合法');
        if (!is_numeric($form_state['values']['order_rows'][$i]['amount']))
            form_set_error('order_rows][' . $i . '][amount', '数量不合法');
        if (!is_numeric($form_state['values']['order_rows'][$i]['amount']))
            form_set_error('order_rows][' . $i . '][amount', '单价不合法');
    }
}
