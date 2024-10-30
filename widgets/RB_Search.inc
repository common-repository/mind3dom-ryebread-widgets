<?php
if (!class_exists('RB_Search')):

class RB_Search extends RyeBread_Widget {
	private $items;
	private $redirectto;
	public function HandleAjax(){}
	
	public static function Install(){
		rb_register_widget_class("RB_Search");
	}
	
	public static function UnInstall(){
		global $wpdb;
		$class = rb_installed_class(null,"RB_Search");
		$wpdb->query("delete from ".$wpdb->prefix."rbwidgets where classid=".$wpdb->escape($class->id));
		rb_remove_widget_class("RB_Search");
	}
	
	
	public function ConfigPage(){
	}
	
	public function HandleConfigAction($action = null){
	}

	protected function DefaultRendering(){
	?>
	<form method="get" id="searchform" action="<?php echo get_option('home'); ?>" >
	<label class="hidden" for="s"><?php _e('Search for:'); ?></label>
	<div><input type="text" value="<?php echo attribute_escape(apply_filters('the_search_query', get_search_query())); ?>" name="s" id="s" />
	<button onclick="javascript:submit();" id="searchsubmit"><?php _e('Search'); ?></button>
	</div>
	</form>
	<?php	
	}
}

endif;

?>