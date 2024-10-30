<?php
if (!class_exists('RB_RecentPosts')):

class RB_RecentPosts extends RyeBread_Widget {
	private $items;
	public function HandleAjax(){}
	
	public static function Install(){
		rb_register_widget_class("RB_RecentPosts");
		global $wpdb;

		$q = "
create table ".$wpdb->prefix."rbrecentpostswidgets (
`id` int(5) not null,
`category` bigint(20),
`postscount` int,
primary key id (id),
CONSTRAINT `rbrp_fk1` FOREIGN KEY (id) REFERENCES ".$wpdb->prefix."rbwidgets (id) ON DELETE CASCADE
) engine=InnoDB
";
		$wpdb->query($q);		
	}
	
	public static function UnInstall(){
		global $wpdb;
		$class = rb_installed_class(null,"RB_RecentPosts");
		$wpdb->query("delete from ".$wpdb->prefix."rbwidgets where classid=".$wpdb->escape($class->id));
		$wpdb->query("drop table ".$wpdb->prefix."rbrecentpostswidgets");
		rb_remove_widget_class("RB_RecentPosts");
	}
	
	
	public function ConfigPage(){
		global $wpdb;
		$r = $wpdb->get_row("select * from ".$wpdb->prefix."rbrecentpostswidgets where id=".$wpdb->escape($this->Id),OBJECT);		
		?>
	<form action="<?php echo get_option('siteurl')."/wp-admin/admin-ajax.php"; ?>" method="post" ajax="true" responsetype="script">
	 <input type="hidden" name="action" value="rb_admin_call" />
	 <input type="hidden" name="rbadminaction" value="configwidget" />
	 <input type="hidden" name="widget" value="<?php echo $this->Id; ?>" />	
  	<table>
  	<tr><td><?php  _e("Show posts of","RB_RecentPosts"); ?></td><td><select id="category">
  	<?php
  		$p = get_terms("category",array('hide_empty' => false));
  		foreach ($p as $pg)
  			echo '<option value="'.$pg->term_id.'" '.(($pg->term_id == $r->category)?'selected':'').'>'.$pg->name.'</option>';
  	
  	 ?>
  	</select></td></tr>
  	<tr><td><?php _e("Posts count","RB_RecentPosts"); ?></td><td><input type="edit" id="postscount" size="5" value="<?php echo $r->postscount; ?>" /></td></tr>
  	</table>
  	<div style="text-align:right;"><button id="savebtn_<?php echo $this->Id; ?>" onclick="javascript:this.disabled = true;jQuery(this).submit();"><?php _e("Apply"); ?></button></div>
  	</form>
		<?php
	}
	
	public function HandleConfigAction($action = null){
			global $wpdb;
			
			if ($r = $wpdb->get_row("select id from ".$wpdb->prefix."rbrecentpostswidgets where id=".$wpdb->escape($this->Id),OBJECT))	
				$wpdb->update($wpdb->prefix."rbrecentpostswidgets",array("category"=>$wpdb->escape($_REQUEST["category"]),"postscount"=>$wpdb->escape($_REQUEST["postscount"])),array("id"=>$wpdb->escape($this->Id)));
			else
				$wpdb->insert($wpdb->prefix."rbrecentpostswidgets",array("id"=>$wpdb->escape($this->Id),"category"=>$wpdb->escape($_REQUEST["category"]),"postscount"=>$wpdb->escape($_REQUEST["postscount"])));
			?>
			document.getElementById("savebtn_<?php echo $_REQUEST["widget"]; ?>").disabled = false;
			<?php		
	}

	protected function DefaultRendering(){
		$nodes = $this->getItems();
		foreach ($nodes as $node){
			?>
			<div class="rb_rp_post">
			<div class="rb_rp_title"><?php echo $node->post_title; ?></div>
			<div class="rb_rp_post_content"><?php echo $node->post_snippet; ?>&nbsp;<?php echo $node->more_link_needed?'<a class="morelink" href="'.get_permalink($node->ID).'">'.__("More...").'</a>':''; ?></div>
			</div>
			<?php
		}
	}
	
	protected function getItems(){
		global $wp_query;
		if (!isset($this->items)){
			global $wpdb;
			$r = $wpdb->get_row("select * from ".$wpdb->prefix."rbrecentpostswidgets where id=".$wpdb->escape($this->Id),OBJECT);
			
			if (!isset($r->postscount)) $r->postscount = 5;
			
			$get_posts = new WP_Query;
			$r = array(
				'numberposts' => $r->postscount, 'offset' => 0,
				'category' => $r->category, 'orderby' => 'post_date',
				'order' => 'DESC', 'include' => '',
				'exclude' => '', 'meta_key' => '',
				'meta_value' =>'', 'post_type' => 'post',
				'suppress_filters' => true
				);

			if ( empty( $r['post_status'] ) )
				$r['post_status'] = ( 'attachment' == $r['post_type'] ) ? 'inherit' : 'publish';
			if ( ! empty($r['numberposts']) )
				$r['posts_per_page'] = $r['numberposts'];
			if ( ! empty($r['category']) )
				$r['cat'] = $r['category'];
			if ( ! empty($r['include']) ) {
				$incposts = preg_split('/[\s,]+/',$r['include']);
				$r['posts_per_page'] = count($incposts);  // only the number of posts included
				$r['post__in'] = $incposts;
			} elseif ( ! empty($r['exclude']) )
				$r['post__not_in'] = preg_split('/[\s,]+/',$r['exclude']);

			$r['caller_get_posts'] = true;
			$this->items = $get_posts->query($r);
			
			foreach ($this->items as &$item){
				$item->post_snippet = $item->post_content;
				$item->more_link_needed = false;	
				if ( preg_match('/<!--more(.*?)?-->/', $item->post_snippet, $matches) ) {
					$temp = explode($matches[0], $item->post_snippet, 2);
					$item->post_snippet = $temp[0];
					$item->more_link_needed = true;	
				}			
			}
		}
		return $this->items;
	}
}

endif;

load_plugin_textdomain("RB_RecentPosts",null,"widgets");
?>