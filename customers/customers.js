

var products;
jQuery(document).ready(function() {
    jQuery('#datepicker').datepicker();
    Drupal.behaviors.CustomersBehaviour = {
        attach: function(context, settings) {  
            jQuery('#-add-check-form').ajaxComplete(function(event, xhr, settings) {
                refresh_table2();
            });
        }
    }
    refresh_table2()
    get_products();
    product_changed(jQuery('select[name="check_rows[0][specification]"]')[0]);
});

var test;

function refresh_table2(){
    jQuery(".check_form_amount input").unbind("keyup");
    jQuery(".check_form_amount input").unbind("mouseleave");
    jQuery(".check_form_price input").unbind("keyup");
    jQuery(".check_form_price input").unbind("mouseleave");
    jQuery('.check_form_product select').unbind("change");
    jQuery('.check_form_amount input').keyup(function() {
        change_sum2(this);
    });
    jQuery('.check_form_amount input').mouseleave(function() {
        change_sum2(this);
    });
    jQuery('.check_form_price input').keyup(function() {
        change_sum2(this);
    });
    jQuery('.check_form_price input').mouseleave(function() {
        change_sum2(this);
    });
    jQuery('.check_form_product select').change(function(){
        product_changed(this);
    });
    jQuery('.check_form_date input').datepicker();
}

function get_input2(row,name){
    var input = 'input[name="check_rows['+row+']['+name+']"]';
    return jQuery(input);
}

function change_sum2(input){
    var changed = input.name;
    var row = changed.split('[')[1];
    row = row.substr(0,row.length-1);
    row = parseInt(row);
    if(get_input2(row,'price').val() != "" && get_input2(row,'amount').val())
        get_input2(row,'sum').val(
            Math.round((parseFloat(get_input2(row,'price').val()) * parseFloat(get_input2(row,'amount').val())) * 100) / 100
            );
    else{
        get_input2(row,'sum').val(0.00);
    }
}


function get_products(){
    var url = window.location.href;
    url = url.substr(0,url.length-8);
    jQuery.getJSON(url+'products/xml', function(data) {
        products = data;
    });
}

function product_changed(input){
    var changed = input.name;
    var row = changed.split('[')[1];
    row = row.substr(0,row.length-1);
    row = parseInt(row);
    var product = input.value;
    var selected;
    jQuery.each(products,function(index,item){
        if(item.specification == product)
            selected = item.price;
    });
    var price = 'input[name="check_rows['+row+'][price]"]';
    jQuery(price).val(selected);
    change_sum2(input);
}

