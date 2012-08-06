<?php

function _order_render($order) {
    $edit = l('编辑订单', 'vendors/orders/' . $order->order_id . '/edit').'<br/>'
            .l('删除订单', 'vendors/orders/' . $order->order_id . '/delete');
    $result = db_query('select * from {e_vendors_order_products} where order_id = :order_id', array(':order_id' => $order->order_id));
    if ($result->rowCount() == 0) {
        return $edit . '订单为空';
    };
    $header = array('品名', '规格', '单位', '数量', '单价', '合计', '交货日期', '交货地点');
    $rows = array();
    $sum = 0.0;
    foreach ($result as $record) {
        $rows[] = array(
            array('data' => $record->product_name),
            array('data' => $record->size),
            array('data' => $record->unit),
            array('data' => $record->amount),
            array('data' => $record->price),
            array('data' => $record->sum),
            array('data' => $record->delivery_date),
            array('data' => $record->place),
        );
        $sum += $record->sum;
    }
    return $edit
            . '<br/>日期'
            . $order->date
            . theme('table', array('header' => $header, 'rows' => $rows))
            . '订单总计'
            . $sum
            . '<br/>备注: ' . $order->remarks
            . '<br/>'
            . l('导出为电子表格', 'vendors/orders/' . $order->order_id . '/excel');
}