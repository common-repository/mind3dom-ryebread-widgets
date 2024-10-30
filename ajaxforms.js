jQuery(document).ready(function(){
	var forms = document.getElementsByTagName("form");
	for (i=0; i < forms.length; i++){
		ajax = forms[i].getAttribute("ajax") == "true";
		if (ajax){
			target_element = forms[i].getAttribute("targetelement");
			response_type = forms[i].getAttribute("responsetype");
			callback = forms[i].getAttribute("callback");
			jQuery(forms[i]).ajaxForm({target:target_element?'#'+target_element:null,dataType:(response_type)?response_type:null,success:(callback)?new Function("data",callback):null});
			jQuery(forms[i]).append(jQuery('<input type="hidden" name="resp_data_type" value="'+response_type+'" />'));
		}
	}
})

