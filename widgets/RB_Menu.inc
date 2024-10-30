<?php
if (!class_exists('RB_Menu')):

class RB_Menu extends RyeBread_Widget {
	private $items;
	public function HandleAjax(){}
	
	public static function Install(){
		rb_register_widget_class("RB_Menu");
		global $wpdb;

		$q = "
create table ".$wpdb->prefix."rbmenuwidgets (
`id` int(5) not null,
`parent_page` bigint(20),
`showpages` text,
`hidepages` text,
primary key id (id),
CONSTRAINT `rbmw_fk1` FOREIGN KEY (id) REFERENCES ".$wpdb->prefix."rbwidgets (id) ON DELETE CASCADE
) engine=InnoDB
";
		$wpdb->query($q);		
	}
	
	public static function UnInstall(){
		global $wpdb;
		$class = rb_installed_class(null,"RB_Menu");
		$wpdb->query("delete from ".$wpdb->prefix."rbwidgets where classid=".$wpdb->escape($class->id));
		$wpdb->query("drop table ".$wpdb->prefix."rbmenuwidgets");
		rb_remove_widget_class("RB_Menu");
	}
	
	
	public function ConfigPage(){
		global $wpdb;
		$r = $wpdb->get_row("select * from ".$wpdb->prefix."rbmenuwidgets where id=".$wpdb->escape($this->Id),OBJECT);		
		?>
	<form action="<?php echo get_option('siteurl')."/wp-admin/admin-ajax.php"; ?>" method="post" ajax="true" responsetype="script">
	 <input type="hidden" name="action" value="rb_admin_call" />
	 <input type="hidden" name="rbadminaction" value="configwidget" />
	 <input type="hidden" name="widget" value="<?php echo $this->Id; ?>" />	
  	<table>
  	<tr><td><?php  _e("Show subpages of"); ?></td><td><select id="parentpage">
  	<option value="-1" <?php if ($r->parent_page == -1) echo "selected"; ?>><?php _e("current page","RB_Menu"); ?></option>
  	<option value="0" <?php if (!$r->parent_page) echo "selected"; ?>><?php _e("home page","RB_Menu"); ?></option>
  	<?php
  		$p = get_pages();
  		foreach ($p as $pg)
  			echo '<option value="'.$pg->ID.'" '.(($pg->ID == $r->parent_page)?'selected':'').'>'.$pg->post_title.'</option>';
  	
  	 ?>
  	</select></td></tr>
  	<tr><td><?php _e("Show pages","RB_Menu"); ?></td><td><textarea id="showpages" rows="4" cols="20"><?php echo $r->showpages; ?></textarea></td></tr>
  	<tr><td><?php _e("Hide pages","RB_Menu"); ?></td><td><textarea id="hidepages" rows="4" cols="20"><?php echo $r->hidepages; ?></textarea></td></tr>
  	</table>
  	<div style="text-align:right;"><button id="savebtn_<?php echo $this->Id; ?>" onclick="javascript:this.disabled = true;jQuery(this).submit();"><?php _e("Apply"); ?></button></div>
  	</form>
		<?php
	}
	
	public function HandleConfigAction($action = null){
			global $wpdb;
			$parent_page = $_REQUEST["parentpage"];
			if (!$parent_page) $parent_page = 0;
			
			if ($r = $wpdb->get_row("select id from ".$wpdb->prefix."rbmenuwidgets where id=".$wpdb->escape($this->Id),OBJECT))	
				$wpdb->update($wpdb->prefix."rbmenuwidgets",array("parent_page"=>$wpdb->escape($parent_page),"showpages"=>$wpdb->escape($_REQUEST["showpages"]),"hidepages"=>$wpdb->escape($_REQUEST["hidepages"])),array("id"=>$wpdb->escape($this->Id)));
			else
				$wpdb->insert($wpdb->prefix."rbmenuwidgets",array("id"=>$wpdb->escape($this->Id),"parent_page"=>$wpdb->escape($parent_page),"showpages"=>$wpdb->escape($_REQUEST["showpages"]),"hidepages"=>$wpdb->escape($_REQUEST["hidepages"])));
			?>
			document.getElementById("savebtn_<?php echo $_REQUEST["widget"]; ?>").disabled = false;
			<?php		
	}

	protected function DefaultRendering(){
		$nodes = $this->getItems();
		?>
		<ul class="rbmenu">
		<?php
		foreach ($nodes as $node){
		?><li><a href="<?php echo $node->link; ?>"><?php echo $node->name; ?></a></li><?php 	
		}
		?></ul><?php
	}
	
	protected function getItems(){
		global $wp_query;
		if (!isset($this->items)){
			$this->items = array();
			global $wpdb;
			$r = $wpdb->get_row("select * from ".$wpdb->prefix."rbmenuwidgets where id=".$wpdb->escape($this->Id),OBJECT);
			if ($r->parent_page == -1){
				if ($wp_query->is_page)
					$r->parent_page = $wp_query->query_vars["page_id"];
				if ($wp_query->is_category)
					$r->parent_page = $wp_query->query_vars["cat"];
			}
			else if (!$r->parent_page) 
				$r->parent_page = 0;

			$showall = !$r->showpages || trim($r->showpages) == "*";
			$sp = explode(";",$r->showpages);
			$hp = explode(";",$r->hidepages);	
				
			if (($wp_query->is_page) || ($r->parent_page == 0)){
				$pgs = $wpdb->get_results("select * from ".$wpdb->posts." where post_type='page' and post_parent=".$wpdb->escape($r->parent_page)." order by menu_order",OBJECT);
				if (is_array($pgs))
				foreach ($pgs as $p){
			  		if (($showall || (in_array($p->post_title,$sp))) && (!in_array($p->post_title,$hp)))
			  		$p->name = $p->post_title;
			  		$p->link = get_page_link($p->ID);
			  		$this->items[] = $p;
				}
			}
			
			if (($wp_query->is_category) || ($r->parent_page == 0)){
				$pgs = $wpdb->get_results("select t.* from ".$wpdb->terms." t inner join ".$wpdb->term_taxonomy." tt on t.term_id = tt.term_id where tt.taxonomy='category' and tt.parent=".$wpdb->escape($r->parent_page),OBJECT);
				if (is_array($pgs))
				foreach ($pgs as $p){
			  	if (($showall || (in_array($p->name,$sp))) && (!in_array($p->name,$hp)))
			  		$p->link = get_category_link($p->term_id);
			  		$this->items[] = $p;
				}
			}
		}
		return $this->items;
	}
}

endif;

load_plugin_textdomain("RB_Menu",null,"widgets");
?>