

jQuery(document).ready(function() {
    jQuery('#datepicker').datepicker();
    refresh_table2();
    Drupal.behaviors.CustomersBehaviour = {
        attach: function(context, settings) {  
            jQuery('#-add-check-form').ajaxComplete(function(event, xhr, settings) {
                refresh_table2();
            });
        }
    }
});
    
function refresh_table2(){
    jQuery(".form_amount input").unbind("keyup");
    jQuery(".form_amount input").unbind("mouseleave");
    jQuery(".form_price input").unbind("keyup");
    jQuery(".form_price input").unbind("mouseleave");
    jQuery('.form_amount input').keyup(function() {
        change_sum(this);
    });
    jQuery('.form_amount input').mouseleave(function() {
        change_sum(this);
    });
    jQuery('.form_price input').keyup(function() {
        change_sum(this);
    });
    jQuery('.form_price input').mouseleave(function() {
        change_sum(this);
    });
    jQuery('.form_date input').datepicker();
}

function get_input(row,name){
    var input = 'input[name="order_rows['+row+']['+name+']"]';
    return jQuery(input);
}

function change_sum(input){
    var changed = input.name;
    var row = changed.split('[')[1];
    row = row.substr(0,row.length-1);
    row = parseInt(row);
    if(get_input(row,'price').val() != "" && get_input(row,'amount').val())
        get_input(row,'sum').val(
            Math.round((parseFloat(get_input(row,'price').val()) * parseFloat(get_input(row,'amount').val())) * 100) / 100
            );
    else{
        get_input(row,'sum').val(0.00);
    }
}

