/*
coded by Misa
*/

var Last_time_received;
var Time_checking_interval = 5000; //msec
var Timeout = 10000; //msec 
var WSconnection;



var timerId = setInterval(function() {
       var dt_now = Date.now();

	var diff = dt_now - Last_time_received;
	if(diff > Timeout){
		console.log('Timeout detected! at ' + new Date().toTimeString().replace(/ .*/, '') );
		RunSocket();
	}

}, Time_checking_interval);





function LoadContent(_url){
	$.get(_url, function(data) {
		$('#content').html(data);
		//подписка на клик после загрузки контента
		$("#set_new_temp_bot").on("click", function() {
			var new_temp_bot = $('#new_temp_bot').val();
			ChangeSetTemp('cmd?new_temp_bot='+ new_temp_bot);
//			SendSocket('new_temp_bot&'+ new_temp_bot);
			$('#new_temp_bot').val("");
		});
		$("#set_new_temp_delta").on("click", function() {
			var new_temp_delta = $('#new_temp_delta').val();
			ChangeSetTemp('cmd?new_temp_delta='+ new_temp_delta);
//			SendSocket('new_temp_delta&'+ new_temp_delta);
			$('#new_temp_delta').val("");
		});
		$("#set_new_temp_top").on("click", function() {
			var new_temp_top = $('#new_temp_top').val();
			ChangeSetTemp('cmd?new_temp_top='+ new_temp_top);
//			SendSocket('new_temp_top&'+ new_temp_top);
			$('#new_temp_top').val("");
		});
		$("#sel_valve_mode_on_save").on("click", function() {
			SelectValveBtnOnClickHandler(this.id);
		});
		$("#sel_valve_mode_off_save").on("click", function() {
			SelectValveBtnOnClickHandler(this.id);
		});

	});
	GetInitValue();

}

function SelectValveBtnOnClickHandler(btn_id){

	if(btn_id == "sel_valve_mode_on_save"){

		var h = $('#new_sel_valve_on_hour').val();
		var m = $('#new_sel_valve_on_min').val();
		var s = $('#new_sel_valve_on_sec').val();
		if(h != ""){
			ChangeSetTemp('set_valve_t?sel_v_on_hour='+ h);
//			SendSocket('set_valve_t&sel_v_on_hour='+ h);
			$('#new_sel_valve_on_hour').val("");
		} 
		if(m != ""){
			ChangeSetTemp('set_valve_t?sel_v_on_min='+ m);
//			SendSocket('set_valve_t&sel_v_on_min='+ m);
			$('#new_sel_valve_on_min').val("");
		} 
		if(s != ""){
			ChangeSetTemp('set_valve_t?sel_v_on_sec='+ s);
//			SendSocket('set_valve_t&sel_v_on_sec='+ s);
			$('#new_sel_valve_on_sec').val("");
		} 

	} else if (btn_id == "sel_valve_mode_off_save"){

		var h = $('#new_sel_valve_off_hour').val();
		var m = $('#new_sel_valve_off_min').val();
		var s = $('#new_sel_valve_off_sec').val();
		if(h != ""){
			ChangeSetTemp('set_valve_t?sel_v_off_hour='+ h);
//			SendSocket('set_valve_t&sel_v_off_hour='+ h);
			$('#new_sel_valve_off_hour').val("");
		} 
		if(m != ""){
			ChangeSetTemp('set_valve_t?sel_v_off_min='+ m);
//			SendSocket('set_valve_t&sel_v_off_min='+ m);
			$('#new_sel_valve_off_min').val("");
		} 
		if(s != ""){
			ChangeSetTemp('set_valve_t?sel_v_off_sec='+ s);
//			SendSocket('set_valve_t&sel_v_off_sec='+ s);
			$('#new_sel_valve_off_sec').val("");	
		} 

	}
	

}


function SendSocket(msg){
	msg += '#';
	console.log('Socket sended: ', msg);
	WSconnection.send(msg);
}


function RunSocket() {
	if(typeof WSconnection !== 'undefined'){
//		if (WSconnection.readyState === WebSocket.OPEN) {
		    WSconnection.close();
//			WSconnection = null;
//		}
	}
	WSconnection = new WebSocket('ws://' + location.hostname + ':81/',['arduino']);
	// WSconnection = new WebSocket("ws://192.168.0.120:81/", ['arduino']);
	WSconnection.onopen = function () {
		WSconnection.send('Connect from ' + location.hostname);
	};
	WSconnection.onerror = function (error) {
		console.log('WebSocket Error ', error);
	};
	WSconnection.onmessage = function (e) {
		SocketOnMsg(e);
	};
}



function SocketOnMsg(msg){


	var socket_data ="";
	try {
		socket_data = JSON.parse(msg.data);
	} 
	catch (err) {
		console.log('Socket parse error!');
	}

	if(!!!socket_data.time){//если это не время
		var dt = new Date();
		var d = dt.getHours() + ':' + dt.getMinutes() + ':' + dt.getSeconds() + '.' + dt.getMilliseconds();
		console.log('Socket received: ',  socket_data, d);

	} 

	DrawReceivedData(socket_data);

}



function ChangeSetTemp(param){
	$.get(param, function(data) {
		DrawReceivedData(data);
	});

//	SendSocket('set_mode&'+ new_mode);
}


function ChangeValveMode(new_mode){
	$.get('cmd?set_mode='+ new_mode, function(data) {
		DrawReceivedData(data);
	});

//	SendSocket('set_mode&'+ new_mode);
}



function SetBrightnessDisp1638(new_val_str){

	var new_val = parseInt(new_val_str);
	if(new_val >= 0 && new_val <= 7){
//		SendSocket('br1638&'+ new_val);
		$.get('cmd?br1638='+ new_val, function(data) {
			DrawReceivedData(data);
		});

	}

}


function GetInitValue(){
	$.get('config.live.json', function(data) {
		DrawReceivedData(data);
		RunSocket();
	});

}

function SelectValveBtnChangeStateHandler(new_data){

		var is_sel_valve_closed = new_data.Is_valve_closed;
		$selValveState = $('#Is_valve_closed');
		
		if(is_sel_valve_closed == '0'){

			$selValveState.html('Откр');
			$selValveState.removeClass('red-eye');
			$selValveState.addClass('green-eye');

		}
		else if(is_sel_valve_closed == '1'){
			$selValveState.html('Закр');
			$selValveState.removeClass('green-eye');
			$selValveState.addClass('red-eye');

		}

}

function DrawReceivedData(data){
	for (var key in data) {	

		if(key == "time"){
			Last_time_received = Date.now();
			var dt = data[key] - 10800; 
//			Last_time_received =  (data[key] - 10800);
			var d = new Date(dt * 1000);
			var ftime = d.toTimeString().replace(/ .*/, '');
			var fdate = function(date){ 
				var day = d.getDate();
				if (day<10) day='0'+day; 
				var month = d.getMonth() + 1;
				if (month<10) month='0'+month; 
				var year = d.getFullYear(); 
				return day + '.' + month + '.' + year; 
			}; 
			$('#' + key).html(ftime);
			$('#date').html(fdate);

		}else if(key == 'Is_valve_closed'){
			SelectValveBtnChangeStateHandler(data);
		} else if (key == 'Mode'){
			//$('#valve_mode').val(data[key]);
			var new_mode = parseInt(data[key]);
			if(new_mode > -1){

				var children = $('#valve_mode_descr').children();

				$(children).each(function(index) {
					if(index === new_mode){
						$(this).addClass('bordered');
					}else{
						$(this).removeClass('bordered');
					}
				});
//				$('#valve_mode_descr').children[new_mode].addClass('bordered');
			}
		} else if (key == 'Disp1638br'){
			$('#Br_1638').val(data[key]);
		} else if (key == 'Count_Down_valve_ON'){
		        $('#pwm_countdown').html(data[key]);
			$('#pwm_countdown').addClass('green-eye');
			$('#pwm_countdown').removeClass('red-eye');
		} else if (key == 'Count_Down_valve_OFF'){
		        $('#pwm_countdown').html(data[key]);
			$('#pwm_countdown').removeClass('green-eye');
			$('#pwm_countdown').addClass('red-eye');
		} else if (key == 'delta_info'){
			var val = data[key];
			if(val == "1"){//закрыто ШИМом по превышению дельты
				$('#Temp_delta').addClass('red-eye');
/*			        $('#delta_info').html("Выкл");
				$('#delta_info').removeClass('green-eye');
				$('#delta_info').addClass('red-eye');
*/
			}else if(val == "0"){//ШИМ не закрывал
				$('#Temp_delta').removeClass('red-eye');
/*			        $('#delta_info').html("Вкл");
				$('#delta_info').addClass('green-eye');
				$('#delta_info').removeClass('red-eye');
*/
			}
		} else if (key == 'tail_came'){
			var val = data[key];
			if(val == "1"){//пришли хвосты и режим хвостами сменен
				$('#Temp_bot').addClass('red-eye');
			}else if(val == "0"){
				$('#Temp_bot').removeClass('red-eye');
			}

		} else {
			$('#' + key).html(data[key]);
		}
	}

}

