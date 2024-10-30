<?php
if (!class_exists('RB_Form')):

class RB_Form extends RyeBread_Widget {
	const FT_HIDDEN = "hidn";
	const FT_LABEL = "lblb";
	const FT_EDITBOX = "edtb";
	const FT_TEXTBOX = "txtb";
	const FT_COMBOBOX = "cmbb";
	const FT_RADIOBOX = "radb";
	const FT_CHECKBOX = "chkb";
	const FT_LISTBOX = "lstb";
	const FT_FILE = "file";
	
	public static $FieldTypes = array(
			self::FT_HIDDEN => "hidden",
			self::FT_LABEL => "label",
			self::FT_EDITBOX => "editbox",
			self::FT_TEXTBOX => "textbox",
			self::FT_COMBOBOX => "combobox",
			self::FT_RADIOBOX => "radiobox",
			self::FT_CHECKBOX => "checkbox",
			self::FT_LISTBOX => "listbox",
			self::FT_FILE => "file upload"
			);
	
	private $fields;
	
	private $OnAfterPost;
	private $Redirect;
	public $Title;
	public $ButtonText;
	public $Is_Ajax;
	public $FieldId;
	public $ResponseType;
	public $TargetElement;
	public $CallBack;
	public static $Ajax_Loaded = false;
	
	public function __construct($id,$name,$skin=""){
		parent::__construct($id,$name,$skin);
		global $wpdb;
		$this->OnAfterPost="";
		$this->Is_Ajax = false;
		$this->Redirect = null;
		if ($r = $wpdb->get_row("select title, is_ajax, responsetype, targetelement, callback, onafterpost, redirect, buttontext from ".$wpdb->prefix."rbformwidgets where id=".$wpdb->escape($this->Id)))
		{
			$this->Title = $r->title;
			$this->ButtonText = $r->buttontext;
			$this->Is_Ajax = $r->is_ajax;
			$this->OnAfterPost = $r->onafterpost;
			$this->ResponseType = $r->responsetype;
			if ($this->ResponseType == "") $this->ResponseType = null; 
			$this->TargetElement = $r->targetelement;
			if ($this->TargetElement == "") $this->TargetElement = null; 
			$this->CallBack = $r->callback;
			if ($this->CallBack == "") $this->CallBack = null;
			$this->Redirect = $r->redirect; 
			if ($this->Redirect == "") $this->Redirect = null;
		}
	}

	public static function Install(){
		rb_register_widget_class("RB_Form");
		global $wpdb;

		$q = "
		create table ".$wpdb->prefix."rbformwidgets (
		`id` int(5) not null,
		`title` varchar(200),
		`is_ajax` int(1),
		`responsetype` varchar(20),
		`targetelement` varchar(200),
		`callback` text,
		`onafterpost` varchar(200), 
		`redirect` varchar(200),
		`buttontext` varchar(200),
		primary key id (id),
		CONSTRAINT `rbfw_fk1` FOREIGN KEY (id) REFERENCES ".$wpdb->prefix."rbwidgets (id) ON DELETE CASCADE
		) engine=InnoDB
		";
		$wpdb->query($q);

		$q = "
create table ".$wpdb->prefix."rbformfields (
`id` bigint(20) not null auto_increment,
`form` int(5) not null,
`label` varchar(100),
`name` varchar(50),
`type` char(4),
`default` text,
`items` text,
`onfetchitems` varchar(100),
`styles` text,
`attrs` text,
primary key id (id),
CONSTRAINT `rbff_fk1` FOREIGN KEY (form) REFERENCES ".$wpdb->prefix."rbwidgets (id) ON DELETE CASCADE
) engine=InnoDB
";
		$wpdb->query($q);	
	}
	
	public static function UnInstall(){
		global $wpdb;
		$class = rb_installed_class(null,"RB_Form");
		$wpdb->query("delete from ".$wpdb->prefix."rbwidgets where classid=".$wpdb->escape($class->id));
		$wpdb->query("drop table ".$wpdb->prefix."rbformfields");
		$wpdb->query("drop table ".$wpdb->prefix."rbformwidgets");
		rb_remove_widget_class("RB_Form");
	}
	
	
	public function ConfigPage(){
		global $wpdb;
		$r = $wpdb->get_row("select * from ".$wpdb->prefix."rbformwidgets where id=".$wpdb->escape($this->Id),OBJECT);		
		?>
	<form id="rbfconf_<?php echo $this->Name; ?>" action="<?php echo get_option('siteurl')."/wp-admin/admin-ajax.php"; ?>" method="post" ajax="true" responsetype="script">
	 <input type="hidden" name="action" value="rb_admin_call" />
	 <input type="hidden" name="rbadminaction" value="configwidget" />
	 <input type="hidden" name="widget" value="<?php echo $this->Id; ?>" />	
  	<table>
  	<tr><td><?php _e("Title");?></td><td><input type="text" name="title" value="<?php echo $r->title; ?>" size="30"/></td></tr>
  	<tr><td><?php _e("Is ajax form");?></td><td><input type="checkbox" name="isajax" <?php echo ($r->is_ajax == 1?'checked':''); ?>/></td></tr>
  	<tr><td><?php _e("Response is");?></td><td><select name="responsetype">
  					<option value="" <?php echo ((!$r->responsetype) || ($r->responsetype == ""))?"selected":""; ?>>Text</option>
  					<option value="xml" <?php echo ($r->responsetype == "xml")?"selected":""; ?>>XML</option>
  					<option value="script" <?php echo ($r->responsetype == "script")?"selected":""; ?>>JavaScript</option>
  					<option value="json" <?php echo ($r->responsetype == "json")?"selected":""; ?>>JSON</option>
  					</select></td></tr>
  	<tr><td><?php _e("Target element id");?></td><td><input type="text" name="targetelement" size="30" value="<?php echo $r->targetelement; ?>" /></td></tr>
  	<tr><td><?php _e("Javascript callback");?></td><td><textarea name="callback" rows="10" cols="40"><?php echo $r->callback; ?></textarea></td></tr>  	
  	<tr><td><?php _e("Execute script after post");?></td><td><input type="text" name="onafterpost" value="<?php echo $r->onafterpost; ?>" size="40"/></td></tr>
  	<tr><td><?php _e("Redirect to page after post");?></td><td><input type="text" name="redirect" value="<?php echo $r->redirect; ?>" size="40"/></td></tr>
  	<tr><td><?php _e("Button text");?></td><td><input type="text" name="buttontext" value="<?php echo $r->buttontext; ?>" size="30" /></td></tr>
  	</table>
  	<div style="text-align:right;"><button id="savebtn_<?php echo $this->Id; ?>" onclick="javascript:this.disabled = true;jQuery(this).submit();"><?php _e("Apply"); ?></button></div>
  	</form>
	<div class="rb_from_heading"><?php _e("Add field");?></div>
	<form id="rbaddfld_<?php echo $this->Name; ?>" action="<?php echo get_option('siteurl')."/wp-admin/admin-ajax.php"; ?>" method="post" ajax="true" callback="jQuery(&quot;#rbformfields_<?php echo $this->Id; ?>&quot;).append(jQuery(data));">
	 <input type="hidden" name="action" value="rb_admin_call" />
	 <input type="hidden" name="rbadminaction" value="rbformaddfield" />
	 <input type="hidden" name="widget" value="<?php echo $this->Id; ?>" />	
	<div>
	<table>
	<tr><td><?php _e("Name");?></td><td><input type="text" name="name" value="" size="20"/></td></tr>
	<tr><td><?php _e("Label");?></td><td><input type="text" name="label" value="" size="20"/></td></tr>
	<tr><td><?php _e("Type");?></td><td>
		<select name="type">
		<?php
			foreach (self::$FieldTypes as $key=>$value){
				?><option value="<?php echo $key; ?>"><?php echo $value; ?></option><?php
			}
		?>
		</select>
	</td></tr>
	</table>
	<button onclick="jQuery(this).submit();"><?php _e("Add"); ?></button>
	</div>
	</form>
	<div class="rb_from_heading"><?php _e("Form fields");?></div>
  	<div id="rbformfields_<?php echo $this->Id; ?>">
	<?php
		$fields = $wpdb->get_results("select * from ".$wpdb->prefix."rbformfields where form=".$wpdb->escape($this->Id),OBJECT);
		if (is_array($fields))
		foreach ($fields as $field)
			$this->OutFieldConfig($field);
	?>
	
  	</div>
		<?php
		
		
	}
	
	protected function OutFieldConfig($field){
?>
<div id="rbff_<?php echo $field->id; ?>" class="rb_formfield">
	<div class="rb_formfield_title" onclick="javascript:jQuery(&quot;#rbffp_<?php echo $field->id; ?>&quot;).css(&quot;display&quot;,(jQuery(&quot;#rbffp_<?php echo $field->id; ?>&quot;).css(&quot;display&quot;) == &quot;none&quot;)?&quot;block&quot;:&quot;none&quot;);" >
		<table style="width:100%">
			<tr>
				<td>
				<?php echo $field->name; ?>&nbsp;:&nbsp;<?php echo self::$FieldTypes[$field->type]; ?>
				</td>
				<td style="text-align:right;">
					<button onclick="javascript:jQuery.ajax({data: {action:&quot;rb_admin_call&quot;,rbadminaction:&quot;rbformdelfield&quot;,widget:&quot;<?php echo $this->Id; ?>&quot;,field:&quot;<?php echo $field->id; ?>&quot;},type:&quot;POST&quot;,dataType:&quot;script&quot;, url:&quot;<?php echo get_option('siteurl')."/wp-admin/admin-ajax.php";  ?>&quot;});"><?php _e("Delete"); ?></button>
				</td>
			</tr>
		</table>
	</div>
	<div id="rbffp_<?php echo $field->id; ?>" class="rb_formfield_properties" style="display:none;">
  		<form action="<?php echo get_option('siteurl')."/wp-admin/admin-ajax.php";  ?>" method="post" ajax="true" responsetype="script">
	 		<input type="hidden" name="action" value="rb_admin_call" />
	 		<input type="hidden" name="rbadminaction" value="rbformeditfield" />
	 		<input type="hidden" name="widget" value="<?php echo $this->Id; ?>" />
	 		<input type="hidden" name="field" value="<?php echo $field->id; ?>" />	
  			<table>
  				<tr><td><?php _e("Name");?></td><td><input type="text" name="name" value="<?php echo $field->name; ?>" size="30" /></td></tr>
  				<tr><td><?php _e("Label");?></td><td><input type="text" name="label" value="<?php echo $field->label; ?>" size="30" /></td></tr>
  				<tr><td><?php _e("Type");?></td><td><select name="type">
				<?php
					foreach (self::$FieldTypes as $key=>$value){
						?><option value="<?php echo $key; ?>" <?php echo (($key == $field->type)?'selected':''); ?>><?php echo $value; ?></option><?php
					}
				?>
  						</select></td></tr>
  				<tr><td><?php _e("Default value");?></td><td><input type="text" name="default" value="<?php echo $field->default; ?>" size="30" /></td></tr>
  				<tr><td><?php _e("Items");?></td><td><textarea name="items" cols="20" rows="4"><?php echo $field->items; ?></textarea></td></tr>
  				<tr><td><?php _e("Script for item fetch");?></td><td><input type="text" name="onfetchitems" value="<?php echo $field->onfetchitems; ?>" size="30" /></td></tr>
  				<tr><td><?php _e("Html attributes");?></td><td><textarea name="attrs" cols="20" rows="4"><?php echo $field->attrs; ?></textarea></td></tr>
  				<tr><td><?php _e("Css styles");?></td><td><textarea name="styles" cols="20" rows="4"><?php echo $field->styles; ?></textarea></td></tr>
  			</table>
  			<button id="rbffeb_<?php echo $field->id; ?>" onclick="javascript:this.disabled=true;jQuery(this).submit();"><?php _e("Apply"); ?></button>
  		</form>
	</div>
</div>
		<?php		
	}
	
	public function HandleConfigAction($action = null){
		global $wpdb;
		switch ($action){
			case "configwidget":{
				$is_ajax = ($_REQUEST["isajax"] == "on")?1:0;
				if ($r = $wpdb->get_row("select id from ".$wpdb->prefix."rbformwidgets where id=".$wpdb->escape($this->Id),OBJECT))	
					$wpdb->update($wpdb->prefix."rbformwidgets",array("title"=>$_REQUEST["title"],"is_ajax"=>$wpdb->escape($is_ajax),"responsetype"=>$wpdb->escape($_REQUEST["responsetype"]),"targetelement"=>$wpdb->escape($_REQUEST["targetelement"]),"callback"=>$wpdb->escape($_REQUEST["callback"]),"onafterpost"=>$wpdb->escape($_REQUEST["onafterpost"]),"redirect"=>$wpdb->escape($_REQUEST["redirect"]),"buttontext"=>$wpdb->escape($_REQUEST["buttontext"])),array("id"=>$wpdb->escape($this->Id)));
				else
					$wpdb->insert($wpdb->prefix."rbformwidgets",array("id"=>$wpdb->escape($this->Id),"title"=>$_REQUEST["title"],"is_ajax"=>$wpdb->escape($is_ajax),"responsetype"=>$wpdb->escape($_REQUEST["responsetype"]),"targetelement"=>$wpdb->escape($_REQUEST["targetelement"]),"callback"=>$wpdb->escape($_REQUEST["callback"]),"onafterpost"=>$wpdb->escape($_REQUEST["onafterpost"]),"redirect"=>$wpdb->escape($_REQUEST["redirect"]),"buttontext"=>$wpdb->escape($_REQUEST["buttontext"])));
			?>
			document.getElementById("savebtn_<?php echo $_REQUEST["widget"]; ?>").disabled = false;
			<?php		
			}break;
			case "rbformaddfield":{
				$wpdb->insert($wpdb->prefix."rbformfields",array("form"=>$wpdb->escape($_REQUEST["widget"]),"label"=>$wpdb->escape($_REQUEST["label"]),"name"=>$wpdb->escape($_REQUEST["name"]),"type"=>$wpdb->escape($_REQUEST["type"])));
				$id = $wpdb->insert_id;
				$w = rb_widget(null,$_REQUEST["widget"]);
				$field = $w->getField($id);
				header("Cache-Control:no-cache");
				header("Pragma:no-cache");
				$this->OutFieldConfig($field);
			}break;
			case "rbformdelfield":{
				$wpdb->query("delete from ".$wpdb->prefix."rbformfields where id=".$wpdb->escape($_REQUEST["field"]));
				?>
				jQuery("#rbff_<?php echo $_REQUEST["field"]; ?>").remove();
				<?php
			}break;
			case "rbformeditfield":{
				$wpdb->update($wpdb->prefix."rbformfields",array("label"=>$wpdb->escape($_REQUEST["label"]),"name"=>$wpdb->escape($_REQUEST["name"]),"type"=>$wpdb->escape($_REQUEST["type"]),"default"=>$wpdb->escape($_REQUEST["default"]),"items"=>$wpdb->escape($_REQUEST["items"]),"onfetchitems"=>$wpdb->escape($_REQUEST["onfetchitems"]),"attrs"=>$wpdb->escape($_REQUEST["attrs"]),"styles"=>$wpdb->escape($_REQUEST["styles"])),array("id"=>$wpdb->escape($_REQUEST["field"])));	
				?>
				jQuery("#rbffeb_<?php echo $_REQUEST["field"]; ?>").attr("disabled",false);
				<?php
			}break;
		}
	}	
		
	public function HandleAjax(){
		$this->AjaxProcessPost();
	}
	
	protected function OutFormStart(){
		$ajaxprops = 'ajax="true" action="'.$this->AjaxCall(array(),null).'"'.($this->ResponseType?' responsetype="'.$this->ResponseType.'"':'').($this->TargetElement?' targetelement="'.$this->TargetElement.'"':'').($this->CallBack?' callback="'.htmlspecialchars($this->CallBack).'"':'');
		
		?>
		<form id="rbf_<?php echo $this->Name; ?>" method="post" <?php echo $this->Is_Ajax?$ajaxprops:''; ?>>
		<input type="hidden" name="method" value="AjaxProcessPost" />
		<?php
	}
	
	protected function OutHiddenFields($fields){
		foreach ($fields as $field){
			if ($field->type == self::FT_HIDDEN){
		?><input type="hidden" name="<?php echo $field->name; ?>" value="<?php echo htmlentities2($field->value); ?>" /><?php 	
			}
		}
	}
	
	protected function OutFormSubmit(){
		$onclick = "this.submit();";		
		if ($this->Is_Ajax)
			$onclick='javascript:jQuery(&quot;input[@name=\'method\']&quot;,jQuery(&quot;#rbf_'.$this->Name.'&quot;)).attr(&quot;value&quot;,&quot;AjaxProcessPost&quot;);jQuery(this).submit();';
		?>
		<div class="btnholder"><button onclick="<?php echo $onclick; ?>"><?php echo $this->ButtonText; ?></button></div>
		<?php
	}
	
	protected function OutFormEnd(){
		?>
		</form>
		<script type="text/javascript">
		 function sendrbform<?php echo $this->Id; ?>(){
		 	jQuery("input[@name='method']",jQuery("#rbf_<?php echo $this->Name; ?>")).attr("value","SaveValues");
		 	jQuery('#rbf_<?php echo $this->Name; ?>').submit();
		 }
		 jQuery("#rbf_<?php echo $this->Name; ?> .rbf_editor").onchange = sendrbform<?php echo $this->Id; ?>;
		 jQuery("#rbf_<?php echo $this->Name; ?> .rbf_clickeditor").onclick = sendrbform<?php echo $this->Id; ?>;
		</script>
		<?php
	}
	
	public function Render(){
		$fields = $this->getFields();
		
		$this->OutFormStart();
		$this->OutHiddenFields($fields);
		
		$skin = get_class($this);
		if (isset($this->Skin)) $skin = $this->Skin;
		
		if (!file_exists(TEMPLATEPATH."/".$skin.".tpl")){
			$this->DefaultRendering();
		} else include TEMPLATEPATH."/".$skin.".tpl";
		
		$this->OutFormSubmit();
		$this->OutFormEnd();
	}
	
	protected function getFieldEditor($field){
			$result = "";
			
			$attributes = "";
			foreach ($field->attrs as $key=>$value)
				$attributes .= $key.'="'.$value.'" ';
			$styles = "";	
			foreach ($field->styles as $key=>$value)
				$styles .= $key.':'.$value.';';
			if ($styles != '') $styles = ' style="'.$styles.'"';
			
			$attrsstyles = $attributes.$styles;
			
			switch ($field->type){
				case self::FT_LABEL:{
					$result .='<span id="'.$field->name.'" '.$attrsstyles.'>'.$field->value.'</span>';
				}break;
				case self::FT_HIDDEN:{
					$result .='<input type="hidden" name="'.$field->name.'" value="'.$field->value.'" '.$attrsstyles.'/>';
				}break;
				case self::FT_EDITBOX:{
					$result .='<input class="rbf_editor" type="text" name="'.$field->name.'" value="'.$field->value.'" '.$attrsstyles.'/>';
				}break;
				case self::FT_TEXTBOX:{
					$result .='<textarea class="rbf_editor" name="'.$field->name.'" '.$attrsstyles.'>'.$field->value.'</textarea>';
				}break;
				case self::FT_COMBOBOX:{
					$result .='<select class="rbf_editor" name="'.$field->name.'" '.$attrsstyles.'>';
					foreach ($field->selectitems as $item)
						$result .='<option value="'.$item["value"].'" '.(($item["value"] == $field->value)?'selected':'').'>'.$item["title"].'</option>';
					$result .='</select>';
				}break;
				case self::FT_CHECKBOX:{
					$result .='<input class="rbf_clickeditor" type="checkbox" name="'.$field->name.'" '.($field->value?'checked':'').' '.$attrsstyles.'/>';
				}break;
				case self::FT_RADIOBOX:{
					foreach ($field->selectitems as $item)
						$result .='<input class="rbf_clickeditor" type="radio" name="'.$field->name.'" value="'.$item["value"].'" '.(($item["value"] = $field->value)?"selected":"").' '.$attrsstyles.'/>'.$item["title"].'<br/>';
				}break;
				case self::FT_LISTBOX:{
					$result .='<select class="rbf_editor" name="'.$field->name.'" multiple="multiple" '.$attrsstyles.'>';
					foreach ($field->selectitems as $item)
						$result .='<option value="'.$item["value"].'" '.(($item["value"] == $field->value)?'selected':'').'>'.$item["title"].'</option>';
					$result .='</select>';
				}break;
				case self::FT_FILE:{
					$result .='<input type="file" name="'.$field->name.'" '.$attrsstyles.'/>';
				}break;
			}
			return $result;
	}
	
	protected function DefaultRendering(){
		$fields = $this->fields;
		?>
		<div class="title"><?php echo $this->Title; ?></div>
		<table>
		<?php		
		foreach ($fields as $field)
		if ($field->type != self::FT_HIDDEN)
		{
		?><tr><td><?php echo $field->label; ?></td><td><?php
		 echo $this->getFieldEditor($field);
		?></td></tr><?php
		}
		?>
		</table>
		<?php 
	}
		
	public function SaveValues(){
		@session_start();
		if (!isset($_SESSION[$this->Name."_posted_values"]))
			$_SESSION[$this->Name."_posted_values"] = array();
		$fields = $this->getFields();
		foreach ($fields as $field){
			if (isset($_REQUEST[$field->name])){
				$_SESSION[$this->Name."_posted_values"][$field->name] = $_REQUEST[$field->name];
				if ($_SESSION[$this->Name."_posted_values"][$field->name] == "") unset($_SESSION[$this->Name."_posted_values"][$field->name]);
			} 
		}
	}
	
	public function PostedValue($name){
		@session_start();
		if (isset($_SESSION[$this->Name."_posted_values"]))
			return $_SESSION[$this->Name."_posted_values"][$name];
		return null;
	}
	
	public function __get($name){
		return $this->PostedValue($name);
	}
	
	public function &PostedValues(){
		@session_start();
		$result = array();
		if (isset($_SESSION[$this->Name."_posted_values"])){
			return $_SESSION[$this->Name."_posted_values"];
		}
		return $result;
	}
	
	public function SetSavedValue($name,$value){
		@session_start();
		if (isset($_SESSION[$this->Name."_posted_values"])){
			$_SESSION[$this->Name."_posted_values"][$name] = $value;
		}
	}
	
	public function ProcessPost(){
		$this->AjaxProcessPost();
		if (isset($this->Redirect)){
			$page = get_page_by_title($this->Redirect);
			$link = get_page_link($page->ID);
			header("Location:".$link);
			die;
		}
	}
	
	protected function OnPost(){}
	
	public function AjaxProcessPost(){
		$this->SaveValues();
		if (is_file(ABSPATH.$this->OnAfterPost))
			include ABSPATH.$this->OnAfterPost;
		else
			$this->OnPost();
	}
	
	protected function getField($id, &$field = null){
			if (!isset($field)){
				global $wpdb;
				$field = $wpdb->get_row("select * from ".$wpdb->prefix."rbformfields where id=".$wpdb->escape($id),OBJECT);
			}
			$field->value = $field->default;
			$attrs = explode("\n",$field->attrs);
			$styles = explode("\n",$field->styles);
			$field->attrs = array();
			$field->styles = array();
			foreach ($attrs as $tempitem)
			if ($tempitem != "")
			{
				$temppair = explode(":",$tempitem);
				$field->attrs[$temppair[0]] = $temppair[1];
			}
			foreach ($styles as $tempitem)
			if ($tempitem != "")
			{
				$temppair = explode(":",$tempitem);
				$field->styles[$temppair[0]] = $temppair[1];
			}
			$field->selectitems = array();
			$tempitems = explode("\n",$field->items);
			foreach ($tempitems as $tempitem){
				$temppair = explode(":",$tempitem);
				$field->selectitems[] = array("value"=>$temppair[0],"title"=>$temppair[1]);
			}
			$path = ABSPATH.$field->onfetchitems;
			if (is_file($path))
				include $path;
			return $field;
	}
	
	protected function getFields(){
		if (!isset($this->items)){
			$this->fields = array();
			global $wpdb;
			$this->fields = $wpdb->get_results("select * from ".$wpdb->prefix."rbformfields where form=".$wpdb->escape($this->Id),OBJECT);
			foreach ($this->fields as &$field)
				$this->getField(null,$field);
		}
		
		$pv = $this->PostedValues();
		foreach ($this->fields as &$field){
			if (isset($pv[$field->name]))
				$field->value = $pv[$field->name];
		}
		return $this->fields;
	}
}

endif;

global $plugin_dir;
wp_register_style("rbformsadmincss",plugins_url()."/".$plugin_dir."/widgets/RB_Form/rbform_admin.css");
add_action('admin_init', create_function('', 'wp_enqueue_style("rbformsadmincss");'));

?>