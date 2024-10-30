<?php
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

function rb_register_container($name){
	global $wpdb;
	return $wpdb->insert($wpdb->prefix."rbcontainers",array("name"=>$wpdb->escape($name)));
}
function rb_delete_container($id){
	global $wpdb;
	return $wpdb->query("delete from ".$wpdb->prefix."rbcontainers where id=".$wpdb->escape($id));
}
function rb_rename_container($id,$name){
	global $wpdb;
	return $wpdb->update($wpdb->prefix."rbcontainers",array("name"=>$wpdb->escape($name)),array("id"=>$wpdb->escape($id)));
}
function rb_containers(){
	global $wpdb;
	return $wpdb->get_results("select * from ".$wpdb->prefix."rbcontainers order by name",OBJECT);
}
function rb_container($id){
	global $wpdb;
	return $wpdb->get_row("select * from ".$wpdb->prefix."rbcontainers where id=".$wpdb->escape($id),OBJECT);
}


function rb_register_widget($name,$class){
	global $wpdb;
	return $wpdb->insert($wpdb->prefix."rbwidgets",array("name"=>$wpdb->escape($name),"classid"=>$wpdb->escape($class)));
}
function rb_delete_widget($id){
	global $wpdb;
	return $wpdb->query("delete from ".$wpdb->prefix."rbwidgets where id=".$wpdb->escape($id));
}

function rb_edit_widget($id,$name){
	global $wpdb;
	$wpdb->update($wpdb->prefix."rbwidgets",array("name"=>$wpdb->escape($name)),array("id"=>$wpdb->escape($id)));	
}

function rb_place_widget($sb,$widget,$order){
	global $wpdb;
	return $wpdb->insert($wpdb->prefix."rbsbwidgets",array("sb"=>$wpdb->escape($sb),"widget"=>$wpdb->escape($widget),"order"=>$wpdb->escape($order)));
}

function rb_remove_widget($id){
	global $wpdb;
	return $wpdb->query("delete from ".$wpdb->prefix."rbsbwidgets where id=".$wpdb->escape($id));
}

function rb_edit_placement($id,$order,$skin,$showon,$hideon){
	global $wpdb;
	return $wpdb->update($wpdb->prefix."rbsbwidgets",array("order"=>$wpdb->escape($order),"showonpages"=>$wpdb->escape($showon),"hideonpages"=>$wpdb->escape($hideon),"skin"=>$wpdb->escape($skin)),array("id"=>$wpdb->escape($id)));
}

function rb_container_widgets($container){
	global $wpdb;
	return $wpdb->get_results("select sbw.id,sbw.widget,w.name,wc.classname,sbw.`order`,sbw.showonpages,sbw.hideonpages,sbw.skin from ".$wpdb->prefix."rbsbwidgets sbw inner join ".$wpdb->prefix."rbwidgets w on w.id = sbw.widget inner join ".$wpdb->prefix."rbwidgetclasses wc on wc.id = w.classid where sbw.sb=".$wpdb->escape($container)." order by sbw.`order`,w.name",OBJECT);
}

function rb_container_widget($id){
	global $wpdb;
	return $wpdb->get_row("select sbw.id,sbw.widget,w.name,wc.classname,sbw.`order`,sbw.showonpages,sbw.hideonpages,sbw.skin from ".$wpdb->prefix."rbsbwidgets sbw inner join ".$wpdb->prefix."rbwidgets w on w.id = sbw.widget inner join ".$wpdb->prefix."rbwidgetclasses wc on wc.id = w.classid where sbw.id=".$wpdb->escape($id),OBJECT);
}

function rb_widget_record($id){
	global $wpdb;
	return $wpdb->get_row("select * from ".$wpdb->prefix."rbwidgets where id=".$wpdb->escape($id),OBJECT);
}

function rb_check_widget_class($classname){
	if (!class_exists($classname)){
		$widget_dir = dirname(__FILE__)."/widgets/";
		if (file_exists($widget_dir.$classname.".php"))
			require_once $widget_dir.$classname.".php";
		else if (file_exists($widget_dir.$classname.".inc"))
			require_once $widget_dir.$classname.".inc";
		else if (is_dir($widget_dir.$classname)){
			if (file_exists($widget_dir.$classname."/".$classname.".php"))
				require_once $widget_dir.$classname."/".$classname.".php";
			else if (file_exists($widget_dir.$classname."/".$classname.".inc"))
				require_once $widget_dir.$classname."/".$classname.".inc";
		}
	}
	return class_exists($classname);
}

function rb_load_widget_class($classname){
	if (!class_exists($classname)){
		$widget_dir = dirname(__FILE__)."/widgets/";
		if (file_exists($widget_dir.$classname.".php"))
			include_once $widget_dir.$classname.".php";
		else if (file_exists($widget_dir.$classname.".inc"))
			include_once $widget_dir.$classname.".inc";
		else if (is_dir($widget_dir.$classname)){
			if (file_exists($widget_dir.$classname."/".$classname.".php"))
				include_once $widget_dir.$classname."/".$classname.".php";
			else if (file_exists($widget_dir.$classname."/".$classname.".inc"))
				include_once $widget_dir.$classname."/".$classname.".inc";
		}
	}
	return class_exists($classname);
}



function &rb_widget($name,$id=null,$skin = null){
	global $wpdb;
	$w = null;
	if (isset($id))
	$r = $wpdb->get_row("select w.id,wc.classname from ".$wpdb->prefix."rbwidgets w inner join ".$wpdb->prefix."rbwidgetclasses wc on wc.id = w.classid where w.id=".$wpdb->escape($id),OBJECT);
	else
	$r = $wpdb->get_row("select w.id,wc.classname from ".$wpdb->prefix."rbwidgets w inner join ".$wpdb->prefix."rbwidgetclasses wc on wc.id = w.classid where w.name='".$wpdb->escape($name)."'",OBJECT);
	if (rb_check_widget_class($r->classname)){
		$c = new ReflectionClass($r->classname);
		$w = $c->newInstanceArgs(array("id"=>$r->id,"name"=>$name,"skin"=>$skin));
	}
	return $w;
}

function rb_out_widget($name,$skin){
	$w = rb_widget($name,null,$skin);
	if (isset($w))
		$w->Render();
}

function rb_out_container($name,$start="",$separator="",$end=""){
	global $wpdb;
	global $wp_query;
	$sb = $wpdb->get_row("select id,name from ".$wpdb->prefix."rbcontainers where name='".$wpdb->escape($name)."'",OBJECT);
	
	$widgets = rb_container_widgets($sb->id);
	echo $start;
	$i = 0;
	foreach ($widgets as $widget){
		if ($i > 0) echo $separator;
		$i++;
		$show = true;
		if (!is_null($widget->showonpages))
			if ($widget->showonpages <> "")
				if ($widget->showonpages <> "*"){
					$show = false;
					$sop = explode(";",$widget->showonpages);
					$show = is_page($sop);
				}
				
		if ($show)  
			if (!is_null($widget->hideonpages))
		 		if ($widget->hideonpages <> ""){
		  			if ($widget->showonpages == "*")
		  				$show = false;
		  				else {
		  				$hop = explode(";",$widget->hideonpages);
		  				$show = !is_page($hop);
		  				}
		  			}
		if (!$show) continue;  			
		if (rb_check_widget_class($widget->classname)){
			$c = new ReflectionClass($widget->classname);
			$w = $c->newInstanceArgs(array("id"=>$widget->widget,"name"=>$widget->name,"skin"=>$widget->skin));
			$w->Render();
		}
	}
	echo $end;
}
function rb_widgets(){
	global $wpdb;
	return $wpdb->get_results("select w.*,wc.classname from ".$wpdb->prefix."rbwidgets w inner join ".$wpdb->prefix."rbwidgetclasses wc on wc.id = w.classid order by w.name",OBJECT);
}

function rb_register_widget_class($classname){
	global $wpdb;
	return $wpdb->insert($wpdb->prefix."rbwidgetclasses",array("classname"=>$wpdb->escape($classname)));
}

function rb_remove_widget_class($classname){
	global $wpdb;
	$wpdb->query("delete from ".$wpdb->prefix."rbwidgetclasses where classname='".$wpdb->escape($classname)."'");
}

function rb_widget_classes(){
	$widget_dir = dirname(__FILE__)."/widgets";
	$dir = opendir($widget_dir);
	$result = array();
	while ($fn = readdir($dir)){
		if (($fn != ".") && ($fn != "..")){
			$file = $widget_dir."/".$fn;
			if (is_dir($widget_dir."/".$fn)){
				if (file_exists($widget_dir."/".$fn."/".$fn.".php"))
					$file = $widget_dir."/".$fn."/".$fn.".php";
				else if (file_exists($widget_dir."/".$fn."/".$fn.".inc"))
					$file = $widget_dir."/".$fn."/".$fn.".inc";
			}

			if (is_file($file)){
				$pi = pathinfo($file);
				if ($pi["extension"] == "php" || $pi["extension"] == "inc"){
					if (!class_exists($pi["filename"]))
						require $file;
					if (class_exists($pi["filename"]))
						$c = new ReflectionClass($pi["filename"]);
						if ($c->isSubclassOf("RyeBread_Widget")) 
							$result[$pi["filename"]] = $file;
				}
			}
			
		} 
	}
	return $result;
}

function rb_installed_class($classid,$classname=null){
	global $wpdb;
	if (isset($classid))
	return $wpdb->get_row("select wc.id,wc.classname from ".$wpdb->prefix."rbwidgetclasses wc where wc.id=".$wpdb->escape($classid));
	else
	return $wpdb->get_row("select wc.id,wc.classname from ".$wpdb->prefix."rbwidgetclasses wc where wc.classname=".$wpdb->escape($classname));
}

function rb_installed_widget_classes(){
	global $wpdb;
	return $wpdb->get_results("select * from ".$wpdb->prefix."rbwidgetclasses");
}

function rb_class_widgets($classid,$classname = null){
	global $wpdb;
	if (isset($classid))
	return $wpdb->get_results("select sbw.id,sbw.widget,w.name,wc.classname from ".$wpdb->prefix."rbwidgets w inner join ".$wpdb->prefix."rbwidgetclasses wc on wc.id = w.classid where wc.id=".$wpdb->escape($classid)." order by sbw.`order`,w.name");
	else
	return $wpdb->get_results("select sbw.id,sbw.widget,w.name,wc.classname from ".$wpdb->prefix."rbwidgets w inner join ".$wpdb->prefix."rbwidgetclasses wc on wc.id = w.classid where wc.classname=".$wpdb->escape($classname)." order by sbw.`order`,w.name");
}

abstract class RyeBread_Widget {
	public $Id;
	public $Name;
	protected $Skin;
	
	public function __construct($id,$name,$skin){
		$this->Id = $id;
		$this->Name = $name;
		if (!isset($this->Name)){
			global $wpdb;
			$r = $wpdb->get_row("select * from ".$wpdb->prefix."rbwidgets where id=".$wpdb->escape($this->Id),OBJECT);
			$this->Name = $r->name;
		}
		if (!isset($this->Id)){
			global $wpdb;
			$r = $wpdb->get_row("select * from ".$wpdb->prefix."rbwidgets where name='".$wpdb->escape($this->Name)."'",OBJECT);
			$this->Id = $r->id;
		}
		$this->Skin = $skin;
	}
	
	protected function AjaxCall(array $parameters,$method = null){
		$call = "?rbajaxcall=1&handler=rbah&widget=".$this->Name;
		if (isset($method))
			$call .= "&method=".$method;
		foreach ($parameters as $key=>$value)
			$call .= "&".$key."=".$value; 
		return $call;
	}
	
	abstract public function HandleAjax();
	
	abstract protected function DefaultRendering();
	
	public abstract static function Install();
	
	public abstract static function UnInstall();
	
	
	public abstract function ConfigPage();
	public abstract function HandleConfigAction($action = null);
	
	public function Render(){
		$skin = get_class($this);
		if (isset($this->Skin)) $skin = $this->Skin;
		
		if (!file_exists(TEMPLATEPATH."/".$skin.".tpl")){
			$this->DefaultRendering();
			return;
		}
		include TEMPLATEPATH."/".$skin.".tpl";
	}
}

?>