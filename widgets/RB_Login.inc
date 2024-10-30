<?php
if (!class_exists('RB_Login')):

class RB_Login extends RyeBread_Widget {
	private $items;
	private $redirectto;
	public function HandleAjax(){}
	
	public static function Install(){
		rb_register_widget_class("RB_Login");
		global $wpdb;

		$q = "
create table ".$wpdb->prefix."rbloginformwidgets (
`id` int(5) not null,
`redirectto` text,
primary key id (id),
CONSTRAINT `rblfw_fk1` FOREIGN KEY (id) REFERENCES ".$wpdb->prefix."rbwidgets (id) ON DELETE CASCADE
) engine=InnoDB
";
		$wpdb->query($q);		
		
	}
	
	public static function UnInstall(){
		global $wpdb;
		$class = rb_installed_class(null,"RB_Login");
		$wpdb->query("delete from ".$wpdb->prefix."rbwidgets where classid=".$wpdb->escape($class->id));
		$wpdb->query("drop table ".$wpdb->prefix."rbloginformwidgets");
		rb_remove_widget_class("RB_Login");
	}
	
	
	public function ConfigPage(){
		global $wpdb;
		$r = $wpdb->get_row("select * from ".$wpdb->prefix."rbloginformwidgets where id=".$wpdb->escape($this->Id),OBJECT);		
		?>
	<form action="<?php echo get_option('siteurl')."/wp-admin/admin-ajax.php"; ?>" method="post" ajax="true" responsetype="script">
	 <input type="hidden" name="action" value="rb_admin_call" />
	 <input type="hidden" name="rbadminaction" value="configwidget" />
	 <input type="hidden" name="widget" value="<?php echo $this->Id; ?>" />	
  	<table>
  	<tr><td><?php _e("Redirect to","RB_Login"); ?></td><td><textarea id="redirectto" rows="4" cols="20"><?php echo $r->redirectto; ?></textarea></td></tr>
  	</table>
  	<div style="text-align:right;"><button id="savebtn_<?php echo $this->Id; ?>" onclick="javascript:this.disabled = true;jQuery(this).submit();"><?php _e("Apply"); ?></button></div>
  	</form>
		<?php
	}
	
	public function HandleConfigAction($action = null){
			global $wpdb;
			if ($r = $wpdb->get_row("select id from ".$wpdb->prefix."rbloginformwidgets where id=".$wpdb->escape($this->Id),OBJECT))	
				$wpdb->update($wpdb->prefix."rbloginformwidgets",array("redirectto"=>$wpdb->escape($_REQUEST["redirectto"])),array("id"=>$wpdb->escape($this->Id)));
			else
				$wpdb->insert($wpdb->prefix."rbloginformwidgets",array("id"=>$wpdb->escape($this->Id),"redirectto"=>$wpdb->escape($_REQUEST["redirectto"])));
			?>
			document.getElementById("savebtn_<?php echo $_REQUEST["widget"]; ?>").disabled = false;
			<?php		
	}

	protected function DefaultRendering(){
		if ($this->IsUserLoggedIn()){
			$link = '<a href="' . wp_logout_url() . '">' . __('Log out') . '</a>';
			echo apply_filters('loginout', $link);
			return;
		}
		?>
<form name="loginform" id="loginform" action="<?php echo site_url('wp-login.php', 'login_post') ?>" method="post">
	<p>
		<label><?php _e('Username') ?><br />
		<input type="text" name="log" id="user_login" class="input" value="" size="20" tabindex="10" /></label>
	</p>
	<p>
		<label><?php _e('Password') ?><br />
		<input type="password" name="pwd" id="user_pass" class="input" value="" size="20" tabindex="20" /></label>
	</p>
	<p class="forgetmenot"><label><input name="rememberme" type="checkbox" id="rememberme" value="forever" tabindex="90" /> <?php _e('Remember Me'); ?></label></p>
	<p class="submit">
		<input type="submit" name="wp-submit" id="wp-submit" value="<?php _e('Log In'); ?>" tabindex="100" />
		<input type="hidden" name="redirect_to" value="<?php echo attribute_escape($this->RedirectTo()); ?>" />
		<input type="hidden" name="testcookie" value="1" />
	</p>
</form>
	<?php
		if ($this->UsersCanRegister()){
			$link =  '<a href="' . site_url('wp-login.php?action=register', 'login') . '">' . __('Register') . '</a>';
			echo apply_filters('register', $link);		
		}
	}
	
	protected function UsersCanRegister(){
		return get_option('users_can_register');		
	}
	
	protected function IsUserLoggedIn(){
		return is_user_logged_in();
	}
	
	protected function RedirectTo(){
		global $wpdb;
		if (!isset($this->redirectto)){
			$r = $wpdb->get_row("select * from ".$wpdb->prefix."rbloginformwidgets where id=".$wpdb->escape($this->Id),OBJECT);
			$this->redirectto = $r->redirectto;
		}
		if ($this->redirectto == "") return null;
		if (strtolower($this->redirectto) == "home") return get_option("home"); 
		return $this->redirectto;		
	}	
}

endif;

?>