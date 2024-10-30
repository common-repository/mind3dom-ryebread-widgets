<?php
if (!class_exists('RB_BreadCrumbs')):

class RB_BreadCrumbs extends RyeBread_Widget {
	private $items;
	private $redirectto;
	public function HandleAjax(){}
	
	public static function Install(){
		rb_register_widget_class("RB_BreadCrumbs");
	}
	
	public static function UnInstall(){
		global $wpdb;
		$class = rb_installed_class(null,"RB_BreadCrumbs");
		$wpdb->query("delete from ".$wpdb->prefix."rbwidgets where classid=".$wpdb->escape($class->id));
		rb_remove_widget_class("RB_BreadCrumbs");
	}
	
	
	public function ConfigPage(){
	}
	
	public function HandleConfigAction($action = null){
	}

	protected function DefaultRendering(){
		$path = $this->getPath();
		?>
		<div class="rb_breadcrumbs">
		<?php
		$links = array();
		foreach ($path as $part)
			$links[] = "<a href=\"".$part["link"]."\">".$part["name"]."</a>";
		echo join("&nbsp;|&nbsp;",$links);
		?>
		</div>
		<?php
	}
	
	protected function getPath(){
		global $wp_query;
		$result = array();
		if ($wp_query->is_single || $wp_query->is_category){
			if ($wp_query->is_single){
				$postid = $wp_query->queried_object_id;
				$terms = get_object_term_cache($postid,"category");
				foreach ($terms as $term){
					$cat = get_category($term->term_id);
					$result[] = array("link"=>get_category_link($cat->cat_ID),"name"=>$cat->cat_name);
					break;
				}
			}
			else if ($wp_query->is_category)
				$cat = get_category($wp_query->query_vars["cat"]);
			if (isset($cat))
			while ($cat->parent > 0){
				$cat = get_category($cat->parent);
				$result[] = array("link"=>get_category_link($cat->cat_ID),"name"=>$cat->cat_name);
			}
		} else if ($wp_query->is_page){
				$p = $wp_query->post_parent;
				while ($p > 0){
					$pg = get_page($p);
					$result[] = array("link"=>get_page_link($p),"name"=>$pg->post_title);
					$p = $pg->post_parent;
				}
		}
		$result = array_reverse($result);
		return $result;
	}
}

endif;
?>