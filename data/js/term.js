$(document).ready(function(){
	$.get('term_c.htm', function(data) {
		$('#content').html(data);
		RetrieveTermAddrSet();
		$("#add_new_term_addr").on("click", function() {
		        var new_term_name =  $('#new_term_name').val();
			var new_term_addr = $('#new_term_addr').val();
			if(new_term_name != '' && new_term_addr != ''){
				var url = '/new_term_s?term_name=' + new_term_name + '&term_addr=' + new_term_addr;
				$.get(url, function(data) {
					if(data){
						$('#answ_ok').modal('show');
						RetrieveTermAddrSet();
					}
				});
			} else {
				$('#answ_err').modal('show');				
			}
		});
	});
	$.get('/TermDAJson', function(data) {
		for (var key in data) {	
			$('#new_term_addr').append('<option value="'+data[key]+'" selected="selected">'+data[key]+'</option>');
		}
	});
})

function RetrieveTermAddrSet(){
	$.get("/config.setup.json", function(data) {
		for (var key in data) {	
			$('#' + key).html(data[key]);
		}
	});
}
