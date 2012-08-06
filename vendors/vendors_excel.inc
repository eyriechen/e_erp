<?php

function order_to_excel($order) {
    excel_test($order);
    //return $order->order_id;
}

function excel_test($order) {
    $result = db_query('select * from {e_company_info} limit 1');
    if ($result->rowCount() == 0) {
        return '没有设置公司信息';
    }
    $company = $result->fetch();
    $company_name = $company->name;
    $tel = $company->tel;
    $fax = $company->fax;
    $address = $company->address;

    $result = db_query('select name from e_vendors_orders,e_vendors where e_vendors_orders.vendor_id = e_vendors.vendor_id and order_id = :order_id limit 1', array(':order_id' => $order->order_id));
    $vendor = $result->fetch()->name;

    $path = drupal_get_path('module', 'phpexcel');
    require_once $path . '/PHPExcel.php';
    require_once $path . '/PHPExcel/IOFactory.php';
    $objPHPExcel = new PHPExcel();
    $objPHPExcel->setActiveSheetIndex(0);

    $current_row = 1;
    $objPHPExcel->getActiveSheet()->getRowDimension($current_row)->setRowHeight(30);
    $current_row++;

    $objPHPExcel->getActiveSheet()->setCellValue('A' . $current_row, $company_name);
    $objPHPExcel->getActiveSheet()->getStyle('A' . $current_row)->getFont()->setSize(28);
    $objPHPExcel->getActiveSheet()->getStyle('A' . $current_row)->getAlignment()->setHorizontal(PHPExcel_Style_Alignment::HORIZONTAL_CENTER);
    $objPHPExcel->getActiveSheet()->mergeCells('A' . $current_row . ':I' . $current_row);

    $current_row ++;
    
    $objPHPExcel->getActiveSheet()->getRowDimension($current_row)->setRowHeight(20);
    $current_row ++;

    $objPHPExcel->getActiveSheet()->setCellValue('A' . $current_row, '表格编号:YDVO' . $order->order_id);
    $objPHPExcel->getActiveSheet()->getStyle('A' . $current_row)->getFont()->setSize(12);
    $current_row++;
    $objPHPExcel->getActiveSheet()->setCellValue('A' . $current_row, $company_name);
    $objPHPExcel->getActiveSheet()->getStyle('A' . $current_row)->getFont()->setSize(12);
    $current_row++;
    $objPHPExcel->getActiveSheet()->setCellValue('A' . $current_row, $address);
    $objPHPExcel->getActiveSheet()->getStyle('A' . $current_row)->getFont()->setSize(12);
    $current_row++;
    $objPHPExcel->getActiveSheet()->setCellValue('A' . $current_row, 'TEL: ' . $tel . '   ' . 'FAX: ' . $fax);
    $objPHPExcel->getActiveSheet()->getStyle('A' . $current_row)->getFont()->setSize(12);

    $current_row += 2;
    $objPHPExcel->getActiveSheet()->mergeCells('D' . $current_row . ':F' . $current_row);
    $objPHPExcel->getActiveSheet()->setCellValue('D' . $current_row, '订货单');
    $objPHPExcel->getActiveSheet()->getStyle('D' . $current_row)->getFont()->setSize(42);
    $objPHPExcel->getActiveSheet()->getStyle('D' . $current_row)->getFont()->setBold(true);
    $objPHPExcel->getActiveSheet()->getRowDimension($current_row)->setRowHeight(60);
    $objPHPExcel->getActiveSheet()->getStyle('D' . $current_row)->getAlignment()->setVertical(PHPExcel_Style_Alignment::VERTICAL_CENTER);
    $objPHPExcel->getActiveSheet()->getStyle('D' . $current_row)->getAlignment()->setHorizontal(PHPExcel_Style_Alignment::HORIZONTAL_CENTER);
    $objPHPExcel->getActiveSheet()->getStyle('D' . $current_row)->getBorders()->getTop()->setBorderStyle(PHPExcel_Style_Border::BORDER_THICK);
    $objPHPExcel->getActiveSheet()->getStyle('D' . $current_row)->getBorders()->getLeft()->setBorderStyle(PHPExcel_Style_Border::BORDER_THICK);
    $objPHPExcel->getActiveSheet()->getStyle('D' . $current_row)->getBorders()->getBottom()->setBorderStyle(PHPExcel_Style_Border::BORDER_THICK);
    $objPHPExcel->getActiveSheet()->getStyle('E' . $current_row)->getBorders()->getTop()->setBorderStyle(PHPExcel_Style_Border::BORDER_THICK);
    $objPHPExcel->getActiveSheet()->getStyle('E' . $current_row)->getBorders()->getBottom()->setBorderStyle(PHPExcel_Style_Border::BORDER_THICK);
    $objPHPExcel->getActiveSheet()->getStyle('F' . $current_row)->getBorders()->getTop()->setBorderStyle(PHPExcel_Style_Border::BORDER_THICK);
    $objPHPExcel->getActiveSheet()->getStyle('F' . $current_row)->getBorders()->getBottom()->setBorderStyle(PHPExcel_Style_Border::BORDER_THICK);
    $objPHPExcel->getActiveSheet()->getStyle('F' . $current_row)->getBorders()->getRight()->setBorderStyle(PHPExcel_Style_Border::BORDER_THICK);

    $current_row += 2;

    $objPHPExcel->getActiveSheet()->setCellValue('A' . $current_row, 'TO: ' . $vendor);
    $objPHPExcel->getActiveSheet()->setCellValue('G' . $current_row, 'DATE: ' . $order->date);
    $objPHPExcel->getActiveSheet()->getStyle('A' . $current_row)->getFont()->setSize(12);
    $objPHPExcel->getActiveSheet()->getStyle('A' . $current_row)->getFont()->setBold(true);
    $objPHPExcel->getActiveSheet()->getStyle('G' . $current_row)->getFont()->setSize(12);

    get_order_rows($objPHPExcel, $current_row, $order->order_id);

    $current_row++;
    $objPHPExcel->getActiveSheet()->setCellValue('A' . $current_row, '确认订单后，请回传，谢谢');
    
    if ($order->remarks != '') {
        $current_row++;
        $objPHPExcel->getActiveSheet()->setCellValue('A' . $current_row, '注：' . $order->remarks);
    }

//    if ($current_row < 30) {
//        $current_row = 30;
//    }  else {
        $current_row += 5;
//    }


    $objPHPExcel->getActiveSheet()->setCellValue('A' . $current_row, $company_name);
    $objPHPExcel->getActiveSheet()->getStyle('A' . $current_row)->getFont()->setSize(14);
    $objPHPExcel->getActiveSheet()->mergeCells('A' . $current_row . ':I' . $current_row);
    $objPHPExcel->getActiveSheet()->getStyle('A' . $current_row)->getAlignment()->setHorizontal(PHPExcel_Style_Alignment::HORIZONTAL_RIGHT);
    $current_row++;
    
    $order_date = explode('-',$order->date);
    $order_date_str = $order_date[0].'年'.$order_date[1].'月'.$order_date[2].'日';
    $objPHPExcel->getActiveSheet()->setCellValue('A' . $current_row, $order_date_str);
    $objPHPExcel->getActiveSheet()->getStyle('A' . $current_row)->getFont()->setSize(14);
    $objPHPExcel->getActiveSheet()->mergeCells('A' . $current_row . ':I' . $current_row);
    $objPHPExcel->getActiveSheet()->getStyle('A' . $current_row)->getAlignment()->setHorizontal(PHPExcel_Style_Alignment::HORIZONTAL_RIGHT);

    $objPHPExcel->getActiveSheet()->getColumnDimension('A')->setWidth(6);
    $objPHPExcel->getActiveSheet()->getColumnDimension('B')->setWidth(17);
    $objPHPExcel->getActiveSheet()->getColumnDimension('C')->setWidth(6);
    $objPHPExcel->getActiveSheet()->getColumnDimension('D')->setWidth(6);
    $objPHPExcel->getActiveSheet()->getColumnDimension('E')->setWidth(8);
    $objPHPExcel->getActiveSheet()->getColumnDimension('F')->setWidth(14);
    $objPHPExcel->getActiveSheet()->getColumnDimension('G')->setWidth(13);
    $objPHPExcel->getActiveSheet()->getColumnDimension('H')->setWidth(10);
    $objPHPExcel->getActiveSheet()->getColumnDimension('I')->setWidth(10);

    $objPHPExcel->getActiveSheet()->setTitle('订单');
    header('Content-Type: application/vnd.ms-excel');
    header('Content-Disposition: attachment;filename="eyrie.xls"');
    header('Cache-Control: max-age=0');
    $objWriter = PHPExcel_IOFactory::createWriter($objPHPExcel, 'Excel5');
    $objWriter->save('php://output');
}

function get_order_rows(&$objPHPExcel, &$current_row, $id) {
    $result = db_query('select * from {e_vendors_order_products} where order_id = :order_id', array(':order_id' => $id));
    $current_row++;
    set_cell($objPHPExcel, 'A', $current_row, '序号');
    set_cell($objPHPExcel, 'B', $current_row, '品名');
    set_cell($objPHPExcel, 'C', $current_row, '规格');
    set_cell($objPHPExcel, 'D', $current_row, '单位');
    set_cell($objPHPExcel, 'E', $current_row, '数量');
    set_cell($objPHPExcel, 'F', $current_row, '单价(含税价)');
    set_cell($objPHPExcel, 'G', $current_row, '合计');
    set_cell($objPHPExcel, 'H', $current_row, '交货日期');
    set_cell($objPHPExcel, 'I', $current_row, '交货地点');
    $objPHPExcel->getActiveSheet()->getRowDimension($current_row)->setRowHeight(15.75);

    $num = 0;
    foreach ($result as $record) {
        $current_row++;
        $num++;
        set_cell($objPHPExcel, 'A', $current_row, $num);
        set_cell($objPHPExcel, 'B', $current_row, $record->product_name);
        set_cell($objPHPExcel, 'C', $current_row, $record->size);
        set_cell($objPHPExcel, 'D', $current_row, $record->unit);
        set_cell($objPHPExcel, 'E', $current_row, $record->amount);
        set_cell($objPHPExcel, 'F', $current_row, $record->price);
        set_cell($objPHPExcel, 'G', $current_row, $record->sum);
        set_cell($objPHPExcel, 'H', $current_row, $record->delivery_date);
        set_cell($objPHPExcel, 'I', $current_row, $record->place);
    }
}

function set_cell(&$objPHPExcel, $current_col, $current_row, $data) {
    $objPHPExcel->getActiveSheet()->setCellValue($current_col . $current_row, $data);
    $objPHPExcel->getActiveSheet()->getStyle($current_col . $current_row)->getAlignment()->setWrapText(true);
    $objPHPExcel->getActiveSheet()->getStyle($current_col . $current_row)->getFont()->setSize(12);
    $objPHPExcel->getActiveSheet()->getStyle($current_col . $current_row)->getBorders()->getTop()->setBorderStyle(PHPExcel_Style_Border::BORDER_THIN);
    $objPHPExcel->getActiveSheet()->getStyle($current_col . $current_row)->getBorders()->getLeft()->setBorderStyle(PHPExcel_Style_Border::BORDER_THIN);
    $objPHPExcel->getActiveSheet()->getStyle($current_col . $current_row)->getBorders()->getRight()->setBorderStyle(PHPExcel_Style_Border::BORDER_THIN);
    $objPHPExcel->getActiveSheet()->getStyle($current_col . $current_row)->getBorders()->getBottom()->setBorderStyle(PHPExcel_Style_Border::BORDER_THIN);
}

function excel() {
    $path = drupal_get_path('module', 'phpexcel');
    require_once $path . '/PHPExcel.php';
    require_once $path . '/PHPExcel/IOFactory.php';
    $objPHPExcel = new PHPExcel();
    $objPHPExcel->setActiveSheetIndex(0);
    $objPHPExcel->getActiveSheet()->setCellValue('A1', '你好');
    $objPHPExcel->getActiveSheet()->mergeCells('A1:I1');
    $objPHPExcel->getActiveSheet()->getStyle('C4')->getBorders()->getTop()->setBorderStyle(PHPExcel_Style_Border::BORDER_THICK);
    $objPHPExcel->getActiveSheet()->setTitle('Name of Sheet 1');
    header('Content-Type: application/vnd.ms-excel');
    header('Content-Disposition: attachment;filename="eyrie.xls"');
    header('Cache-Control: max-age=0');
    $objWriter = PHPExcel_IOFactory::createWriter($objPHPExcel, 'Excel5');
    $objWriter->save('php://output');
}