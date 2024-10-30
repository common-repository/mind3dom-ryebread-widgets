<?php
/*
Plugin Name: Mind3doM RyeBread Widgets
Plugin URI: http://www.mind3dom.com/
Description: Performs advanced container and widget application. Allows skinned widgets.
Author: Mind3doM
Author URI: http://www.mind3dom.com/
Version: 1.0
Last Update: 02.02.2009
*/

/*
	Copyright © 2009 Daniel A. Krasilnikov (Mind3doM)

	This file is part of "Mind3doM RyeBread Widgets".

	"Mind3doM RyeBread Widgets" is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	"Mind3doM RyeBread Widgets" is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with "Mind3doM RyeBread Widgets".  If not, see <http://www.gnu.org/licenses/>.
*/

require_once dirname(__FILE__)."/ryebread_functions.php";


if (!function_exists('rb_uninstall')):
function rb_uninstall(){
	global $wpdb;
	$wpdb->query("drop table ".$wpdb->prefix."rbsbwidgets");
	$wpdb->query("drop table ".$wpdb->prefix."rbcontainers");
	$wpdb->query("drop table ".$wpdb->prefix."rbwidgets");
	$wpdb->query("drop table ".$wpdb->prefix."rbwidgetclasses");
}
endif;

if (!function_exists('rb_install')):
function rb_install(){
global $wpdb;
$q = "
create table ".$wpdb->prefix."rbcontainers (
id int(5) not null auto_increment,
name varchar(100) not null,
primary key id (id),
unique key name (name)
) engine=InnoDB
";
$wpdb->query($q);

$q = "
create table ".$wpdb->prefix."rbwidgetclasses (
id int(5) not null auto_increment,
classname varchar(100) not null,
primary key id (id),
unique key classname (classname)
) engine=InnoDB
";
$wpdb->query($q);

$q = "
create table ".$wpdb->prefix."rbwidgets (
id int(5) not null auto_increment,
name varchar(100) not null,
classid int(5) not null,
primary key id (id),
unique key name (name),
CONSTRAINT `rbwc_fk1` FOREIGN KEY (classid) REFERENCES ".$wpdb->prefix."rbwidgetclasses (id) ON DELETE CASCADE
) engine=InnoDB
";
$wpdb->query($q);

$q = "
create table ".$wpdb->prefix."rbsbwidgets (
id int(5) not null auto_increment,
sb int(5) not null,
widget int(5) not null,
`order` int(4),
showonpages text,
hideonpages text,
skin varchar(100),
primary key id (id),
CONSTRAINT `rbsbw_fk1` FOREIGN KEY (sb) REFERENCES ".$wpdb->prefix."rbcontainers (id) ON DELETE CASCADE,
CONSTRAINT `rbsbw_fk2` FOREIGN KEY (widget) REFERENCES ".$wpdb->prefix."rbwidgets (id) ON DELETE CASCADE
) engine=InnoDB
";
$wpdb->query($q);
}
endif;

if (!function_exists('rb_handle_action')):
function rb_handle_action(){
	global $plugin_page;
	if (isset($_REQUEST["rbaction"])){
		switch ($_REQUEST["rbaction"]){
			case "awdelcontainer":aw_handle_delcontainer();break;
			case "aweditcontainer":aw_rename_container($_REQUEST["container"],$_REQUEST["sb_name"]);break;
			case "awplacewidget":aw_place_widget($_REQUEST["container"],$_REQUEST["sbwidget"],$_REQUEST["order"]);break;
			case "awdelwidget":aw_remove_widget($_REQUEST["wid"]);break;
			case "aweditsbwidget":aw_edit_placement($_REQUEST["widget"],$_REQUEST["order"],$_REQUEST["skin"],$_REQUEST["showon"],$_REQUEST["hideon"]);break;
			case "rbcreatewidget":rb_register_widget($_REQUEST["wname"],$_REQUEST["wclass"]);break;
			case "rbdefinecontainer":rb_register_container($_REQUEST["cname"]);break;
			case "rbplacewidget":rb_place_widget($_REQUEST["container"],$_REQUEST["widget"],0);break;
			default:do_action('handle_rbaction');break;
		}
		$redirect = "Location:?page=".$plugin_page;
		header($redirect);
		die;
	}
}
endif;

if (!function_exists('rb_container_management')):
function rb_container_management(){
	global $plugin_page;
	$containers = rb_containers();
	$widgets = rb_widgets();
	/*global $wp_scripts;
	var_dump($wp_scripts);*/
	?>
	<script language="javascript">
		function flipConf(id){
		 if (document.getElementById("conf"+id).style.display == "none")
		 	document.getElementById("conf"+id).style.display = "block";
		 	else
		 	document.getElementById("conf"+id).style.display = "none";
		}

		function flipWidgets(id){
		 if (document.getElementById("widgets_"+id).style.display == "none")
		 	document.getElementById("widgets_"+id).style.display = "block";
		 	else
		 	document.getElementById("widgets_"+id).style.display = "none";
		}
		
		function deleteContainer(id){
			document.getElementById("del_"+id).disabled = true;
			jQuery.ajax({
				data: {action:"rb_admin_call",rbadminaction:"delcontainer",container:id},
				type: "POST",
				dataType: "script",
				url: "<?php echo get_option('siteurl')."/wp-admin/admin-ajax.php";  ?>"
				});		
		}
		
		function removeWidget(id){
			document.getElementById("rem_"+id).disabled = true;
			jQuery.ajax({
				data: {action:"rb_admin_call",rbadminaction:"removewidget",widget:id},
				type: "POST",
				dataType: "script",
				url: "<?php echo get_option('siteurl')."/wp-admin/admin-ajax.php";  ?>"
				});		
		}
	</script>
	<div class="man_console">
	<h3><?php _e("Container management","RyeBread"); ?></h3>
	<table class="addopers">
	<tr>
	<td valign="top">
	<h4><?php _e("Place widget","RyeBread"); ?></h4>
	<div>
	<form method="post">
	<input type="hidden" name="rbaction" value="rbplacewidget" />
	<table>
	<tr><td><?php _e("Container","RyeBread"); ?></td><td>
	<select name="container">
	<?php
		foreach ($containers as $container){
		?>
		<option value="<?php echo $container->id; ?>"><?php echo $container->name; ?></option>
		<?php	
		}
	 ?>
	</select> 
	</td></tr>
	<tr><td><?php _e("Widget","RyeBread"); ?></td><td>
	<select name="widget">
	<?php
		foreach ($widgets as $widget){
		?>
		<option value="<?php echo $widget->id; ?>"><?php echo $widget->name; ?>:<?php echo $widget->classname; ?></option>
		<?php	
		}
	 ?>
	</select> 
	</td></tr>
	</table>
	<button onclick="submit();"><?php _e("Ok","RyeBread");  ?></button>
	</form>
	</div>
	</td>
	<td  valign="top">
	<h4><?php _e("Define container","RyeBread"); ?></h4>
	<div>
	<form method="post">
	<input type="hidden" name="rbaction" value="rbdefinecontainer" />
	<table>
	<tr><td><?php _e("Name","RyeBread"); ?></td>
	<td><input type="edit" name="cname" value="" size="20" /></td></tr>
	</table>
	<button onclick="submit();"><?php _e("Ok","RyeBread");  ?></button>
	</form>
	</div>
	</div>
	</td>
	</tr>
	</table>
	<?php
	
	foreach ($containers as $container){
		?>
		<div class="container" id="c_<?php echo $container->id; ?>">
		<div class="container_title">
		<table style="width:100%;">
		<tr onclick="javascript:flipWidgets(&quot;<?php echo $container->id; ?>&quot;);">
		<td>
		<?php echo $container->name; ?>
		</td>
		<td style="text-align:right;">
		<button id="del_<?php echo $container->id; ?>" onclick="javascript:deleteContainer(&quot;<?php echo $container->id; ?>&quot;);window.event.cancelBubble = true;"><?php _e("Delete","RyeBread"); ?></button>
		</td>
		</tr>
		</table>
		</div>
		<div id="widgets_<?php echo $container->id; ?>" class="container_widgets" style="display:none;">
		<?php
			$widgets = rb_container_widgets($container->id);
			foreach ($widgets as $w){
				?>
				<div id="w_<?php echo $w->id; ?>" class="widget">
				<div id="wt_<?php echo $w->id; ?>" class="widget_title">
				<table style="width:100%;">
					<tr onclick="javascript:flipConf(&quot;<?php echo $w->id; ?>&quot;);">
						<td>
						<?php echo $w->name; ?>:&nbsp;<?php echo $w->classname; ?>
						</td>
						<td style="text-align:right;">
						<button id="rem_<?php echo $w->id; ?>" onclick="javascript:removeWidget(&quot;<?php echo $w->id; ?>&quot;);window.event.cancelBubble = true;"><?php _e("Delete","RyeBread"); ?></button>
						</td>
					</tr>
				</table>
				</div>
				<div id="conf<?php echo $w->id; ?>" class="widget_conf" style="display:none;">
					<form id="confform<?php echo $w->id; ?>" action="<?php echo get_option('siteurl')."/wp-admin/admin-ajax.php";  ?>" method="post" ajax="true" responsetype="script">
					<input type="hidden" name="action" value="rb_admin_call" />
					<input type="hidden" name="rbadminaction" value="savecontwidget" />
					<input type="hidden" name="widget" value="<?php echo $w->id; ?>" />
					<table>
					<tr><td><?php _e("Order","RyeBread"); ?></td><td><input type="text" id="order" value="<?php echo $w->order; ?>" size="3"/></td></tr>
					<tr><td><?php _e("Skin","RyeBread"); ?></td><td><input type="text" id="skin" value="<?php echo $w->skin; ?>" size="30"/></td></tr>
					<tr><td><?php _e("Show on pages","RyeBread"); ?></td><td><textarea rows="4" cols="30" id="showon"><?php echo $w->showonpages; ?></textarea></td></tr>
					<tr><td><?php _e("Hide on pages","RyeBread"); ?></td><td><textarea rows="4" cols="30" id="hideon"><?php echo $w->hideonpages; ?></textarea></td></tr>
					</table>
					<div style="text-align:right;"><button id="save_<?php echo $w->id; ?>" onclick="javascript:this.disabled = true;jQuery(this).submit();"><?php _e("Apply","RyeBread"); ?></button></div>
					</form>
				</div>
				</div>
				<?php
			}
		 ?>
		</div>
		</div>
		<?php
	}
	
	return;
}
endif;

if (!function_exists('rb_add_menu')):
function rb_add_menu() {
	add_menu_page('RyeBread Widgets', 'RyeBread Widgets', 8, __FILE__, 'rb_container_management');
	add_submenu_page(__FILE__,'Widget List', 'Widget List', 8, dirname(__FILE__)."/rbwidgetman.php");
	add_submenu_page(__FILE__,'Widget Classes', 'Widget Classes', 8, dirname(__FILE__)."/rbclassesman.php");
}
endif;


$AJAX_HANDLERS = array();

if (!function_exists('rb_add_ajax_handler')):
function rb_add_ajax_handler($hname,$hfunction){
	global $AJAX_HANDLERS;
	if (!is_array($AJAX_HANDLERS)){
		$AJAX_HANDLERS = array();
	}
	$AJAX_HANDLERS[$hname] = $hfunction;	
}
endif;


if (!function_exists('rb_call_ajax_handler')):
function rb_call_ajax_handler($handler){
	global $AJAX_HANDLERS;
	if (function_exists($AJAX_HANDLERS[$handler]))
		call_user_func($AJAX_HANDLERS[$handler]);
}
endif;

if (!function_exists('rb_ajax_dispatcher')):
function rb_ajax_dispatcher(){
	if (isset($_REQUEST["rbajaxcall"])){
       	header('Cache-Control:no-cache');
       	header('Pragma:no-cache');
		rb_call_ajax_handler($_REQUEST["handler"]);
		die;
	}
}
endif;

if (!function_exists('rb_ajax_handler')): 
function rb_ajax_handler(){
	if (isset($_REQUEST["widget"])){
		$widget = rb_widget($_REQUEST["widget"]);
		if (isset($_REQUEST["method"])){
			$params = array();
			$system = array('ajaxcall','handler','widget','method');
			foreach ($_REQUEST as $key=>$value)
			  if (!in_array($key,$system))
			  	$params[$key]=$value;
			call_user_method_array($_REQUEST["method"],$widget,$params);
		}
		else
		$widget->HandleAjax();
	}
}
endif;

if (!function_exists('rb_set_ajax_handler')):
function rb_set_ajax_handler(){
	if (function_exists('rb_add_ajax_handler'))
		rb_add_ajax_handler('rbah','rb_ajax_handler');
}
endif;

if (!function_exists('rb_shortcode_widget')):
function rb_shortcode_widget($atts){
	ob_start();
	if (!isset($atts["name"])){
		echo "widget name not specified!";
		return;
	}
	rb_out_widget($atts["name"],$atts["skin"]);
	return ob_get_clean();
}
endif;

if (!function_exists('rb_shortcode_container')):
function rb_shortcode_container($atts){
	ob_start();
	if (!isset($atts["name"])){
		echo "container name not specified!";
		return;
	}
	rb_out_container($atts["name"],$atts["start"],$atts["separator"],$atts["end"]);
	return ob_get_clean();
}
endif;

if (!function_exists('rb_on_admin_ajax_call')):

function rb_on_admin_ajax_call(){
	try {
	switch ($_REQUEST["rbadminaction"]){
		case "install":{
			$classname = $_REQUEST["widget"];
			if (rb_check_widget_class($classname)){
				$c = new ReflectionClass($classname);
				$m = $c->getMethod("Install");
				$m->invoke(null);	
			}
			?>
			document.getElementById("btn_<?php echo $classname ?>").innerHTML = "<?php _e("Uninstall"); ?>";
			document.getElementById("btn_<?php echo $classname ?>").disabled = false;
			document.getElementById("btn_<?php echo $classname ?>").onclick = function(){doUninstall("<?php echo $classname ?>")};
			<?php
		}break;
		case "uninstall":{
			$classname = $_REQUEST["widget"];
			if (rb_check_widget_class($classname)){
				$c = new ReflectionClass($classname);
				$m = $c->getMethod("UnInstall");
				$m->invoke(null);	
			}
			?>
			document.getElementById("btn_<?php echo $classname ?>").innerHTML = "<?php _e("Install"); ?>";
			document.getElementById("btn_<?php echo $classname ?>").disabled = false;
			document.getElementById("btn_<?php echo $classname ?>").onclick = function(){doInstall("<?php echo $classname ?>")};
			<?php
		}break;
		case "delwidget":{
			if (rb_delete_widget($_REQUEST["widget"])){
			?>
			w = document.getElementById("w_<?php echo $_REQUEST["widget"] ?>");
			p = w.parentNode;
			p.removeChild(w);
			<?php
			}
		}break;
		case "editwidget":{
			rb_edit_widget($_REQUEST["widget"],$_REQUEST["wname"]);
		}break;
		case "delcontainer":{
			rb_delete_container($_REQUEST["container"]);
			?>
			document.getElementById("del_<?php echo $_REQUEST["container"]; ?>").disabled = false;
			c = document.getElementById("c_<?php echo $_REQUEST["container"]; ?>");
			p = c.parentNode;
			p.removeChild(c);
			<?php		
		}break;
		case "removewidget":{
			rb_remove_widget($_REQUEST["widget"]);
			?>
			document.getElementById("rem_<?php echo $_REQUEST["widget"]; ?>").disabled = false;
			w = document.getElementById("w_<?php echo $_REQUEST["widget"]; ?>");
			p = w.parentNode;
			p.removeChild(w);
			<?php		
		}break;
		case "savecontwidget":{
			rb_edit_placement($_REQUEST["widget"],$_REQUEST["order"],$_REQUEST["skin"],$_REQUEST["showon"],$_REQUEST["hideon"]);			
			?>
			document.getElementById("save_<?php echo $_REQUEST["widget"]; ?>").disabled = false;
			<?php		
		}break;
		default:{
			$w = rb_widget(null,$_REQUEST["widget"]);
			$w->HandleConfigAction($_REQUEST["rbadminaction"]);
		}break;
	}
	} catch (Exception $e){
		switch ($_REQUEST["resp_data_type"]){
			case "script":{
	?>
	alert("<?php echo "Error:".addslashes($e->getMessage()); ?>");
	<?php
			}break;
			case "json":{
				echo 'error:"'.$e->getMessage().'"';
			}break;
			case "xml":{
				echo "<error><![CDATA[".$e->getMessage()."]]></error>";
			}break;
			default:{echo "_Error_:".$e->getMessage();}break;
		}
	}
}

endif;

if (!function_exists('rb_load_installed_classes')):

function rb_load_installed_classes(){
	$classes = rb_installed_widget_classes();
	foreach ($classes as $class)
		rb_load_widget_class($class->classname);
}

endif;

global $plugin_dir;
$plugin_dir = dirname(plugin_basename(__FILE__));

wp_register_style("ryebreadstyles",plugins_url()."/$plugin_dir/style.css");
wp_register_script("ryebreadajaxforms",plugins_url()."/$plugin_dir/ajaxforms.js");
add_action('init', create_function('', 'wp_enqueue_script("jquery");'));
add_action('init', create_function('', 'wp_enqueue_script("jquery-form");'));
add_action('init', create_function('', 'wp_enqueue_script("ryebreadajaxforms");'));
add_action('admin_init', create_function('', 'wp_enqueue_style("ryebreadstyles");'));
add_action('admin_init', create_function('', 'wp_enqueue_script("scriptaculous");'));
add_action('admin_init', create_function('', 'wp_enqueue_script("jquery");'));
add_action('admin_init', create_function('', 'wp_enqueue_script("jquery-form");'));
add_action('admin_init', create_function('', 'wp_enqueue_script("ryebreadajaxforms");'));
add_action('wp_ajax_rb_admin_call', 'rb_on_admin_ajax_call');
add_action('init', 'rb_ajax_dispatcher');
add_action('admin_init', 'rb_handle_action');

rb_load_installed_classes();

add_action('admin_menu', 'rb_add_menu');
add_action('plugins_loaded','rb_set_ajax_handler');
register_activation_hook(__FILE__, 'rb_install');
register_deactivation_hook(__FILE__, 'rb_uninstall');
add_shortcode("rbwidget","rb_shortcode_widget");
add_shortcode("rbcontainer","rb_shortcode_container");
load_plugin_textdomain("RyeBread");
?>