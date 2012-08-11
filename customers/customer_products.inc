<?php

function products_table($customer) {
    $header = array('产品名称','规格型号', '单位', '价格');
    $result = db_query('select * from {e_customers_products} where customer_id = :customer_id', array(':customer_id' => $customer->customer_id));
    if ($result->rowCount() == 0)
        return '<br/>没有产品';
    $data = array();
    foreach ($result as $record) {
        $data[] = array(
            $record->product_name,
            $record->specification,
            $record->unit,
            $record->price,
        );
    }
    $output = theme('table', array('header' => $header,
        'rows' => $data));
    return $output;
}

function _customer_products_render($customer) {
    return l('编辑产品', 'customers/' . $customer->customer_id . '/products/edit') . products_table($customer);
}

function _customer_products_edit($customer) {
    return drupal_get_form('customer_products_form', $customer);
}

function customer_products_form($form, &$form_state, $customer) {
    $form['#tree'] = TRUE;
    $form_state['customer'] = $customer;

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
        $result = db_query('select * from {e_customers_products} where customer_id = :customer_id', array(':customer_id' => $customer->customer_id));
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
                '#prefix' => '<table id = "order_form"><thead><tr><th>品名</th><th>规格型号</th><th>单位</th><th>单价</th></tr></thead><tr><td class = "form_name">',
                '#suffix' => '</td>',
                '#attributes' => array('size' => "25"),
            );
        } else {
            $form['product_rows'][$i]['name'] = array(
                '#type' => 'textfield',
                '#title' => t('品名'),
                '#prefix' => '<tr><td class="form_name">',
                '#suffix' => '</td>',
                '#attributes' => array('size' => "25"),
            );
        }
        $form['product_rows'][$i]['specification'] = array(
            '#type' => 'textfield',
            '#title' => t('规格型号'),
            '#prefix' => '<td class="form_specification">',
            '#suffix' => '</td>',
            '#attributes' => array('size' => "25"),
        );
        $form['product_rows'][$i]['unit'] = array(
            '#type' => 'textfield',
            '#title' => t('单位'),
            '#prefix' => '<td class="form_unit">',
            '#suffix' => '</td>',
            '#attributes' => array('size' => "25"),
        );
        $form['product_rows'][$i]['price'] = array(
            '#type' => 'textfield',
            '#title' => t('单价'),
            '#prefix' => '<td class="form_price">',
            '#suffix' => '</td></tr>',
            '#attributes' => array('size' => "25"),
        );
    }
    $form['product_rows'][$i - 1]['price']['#suffix'] = '</td></tr></table>';
    
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
        $result = db_query('select * from {e_customers_products} where customer_id = :customer_id', array(':customer_id' => $customer->customer_id));
        $i = 0;
        foreach ($result as $record) {
            $form['product_rows'][$i]['name']['#default_value'] = $record->product_name;
            $form['product_rows'][$i]['specification']['#default_value'] = $record->specification;
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

function customer_products_form_submit($form, &$form_state) {
    $customer = $form_state['customer'];
    db_query('delete from {e_customers_products} where customer_id = :customer_id', array(':customer_id' => $customer->customer_id));
    for ($i = 0; $i < $form_state['row_num']; $i++) {
        db_query('insert into {e_customers_products} (customer_id,product_name,specification,unit,price) values (:customer_id,:product_name,:specification,:unit,:price)', array(
            ':customer_id' => $customer->customer_id,
            ':product_name' => $form_state['values']['product_rows'][$i]['name'],
            ':specification' => $form_state['values']['product_rows'][$i]['specification'],
            ':unit' => $form_state['values']['product_rows'][$i]['unit'],
            ':price' => $form_state['values']['product_rows'][$i]['price'],
        ));
    }
    drupal_set_message('更新成功');
    drupal_goto('customers/' . $customer->customer_id . '/products');
}

function _edit_product_form_submit_validate($form, &$form_state) {
    for($i = 0; $i < $form_state['row_num']; $i++){
        if ($form_state['values']['product_rows'][$i]['name'] == '')
            form_set_error('product_rows][' . $i . '][name', '品名不能为空');
        if ($form_state['values']['product_rows'][$i]['specification'] == '')
            form_set_error('product_rows][' . $i . '][specification', '规格型号不能为空');
        if ($form_state['values']['product_rows'][$i]['unit'] == '')
            form_set_error('product_rows][' . $i . '][unit', '单位不能为空');
        if ($form_state['values']['product_rows'][$i]['price'] == '')
            form_set_error('product_rows][' . $i . '][price', '价格不能为空');
    }
}

function _customer_products_xml($customer) {
//    header('Content-Type: text/xml');
    $result = db_query('select * from e_customers_products where customer_id = :customer_id', array(':customer_id' => $customer->customer_id));
//    echo '<products>';
//    foreach ($result as $record) {
//        echo '<product>';
//        echo $record->product_name;
//        echo '</product>';
//    }
//    echo '</products>';
    $products = array();
    foreach ($result as $record) {
        $products[] = array(
            'name' => $record->product_name,
            'specification' => $record->specification,
            'unit' => $record->unit,
            'price' => $record->price,
        );
    }
    drupal_json_output($products);
}