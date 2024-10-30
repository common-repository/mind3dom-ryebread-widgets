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



	require_once dirname(__FILE__)."/ryebread_functions.php";

	$wc = rb_installed_widget_classes();
	
	$installed = array();
	foreach ($wc as $c)
		$installed[$c->classname] = true;
	
	?>
	<div class="man_console">
	<script language="javascript">
		function classActionCallback(response){
			eval(response);
		}
		
		function doInstall(classname){
			document.getElementById("btn_"+classname).disabled = true;
			jQuery.ajax({
				data: {action:"rb_admin_call",rbadminaction:"install",widget:classname},
				type: "POST",
				url: "<?php echo get_option('siteurl')."/wp-admin/admin-ajax.php";  ?>",
				success: classActionCallback
				});
		}
		
		function doUninstall(classname){
			document.getElementById("btn_"+classname).disabled = true;
			jQuery.ajax({
				data: {action:"rb_admin_call",rbadminaction:"uninstall",widget:classname},
				type: "POST",
				url: "<?php echo get_option('siteurl')."/wp-admin/admin-ajax.php"; ?>",
				success: classActionCallback
				});		
		}
	</script>
	<h3><?php _e("Widget classes management","RyeBread"); ?></h3>
	<table class="rb_class_man_console">
	<col /><col width="1">
	<tr>
		<th valign="top">
		<?php _e("Class","RyeBread"); ?>
		</th>
		<th valign="top">
		<?php _e("Action","RyeBread"); ?>
		</th>
	</tr>
	<?php
		$wclasses = rb_widget_classes();
		$counter = 0;
		foreach ($wclasses as $wclass=>$v){
			?>
			<tr class="item<?php echo $counter % 2; ?>">
			<td>
				<?php echo $wclass; ?>
			</td>
			<td>
				<button id="btn_<?php echo $wclass; ?>" onclick="javascript:do<?php echo $installed[$wclass]?"Uninstall":"Install"; ?>(&quot;<?php echo $wclass; ?>&quot;);"><?php _e($installed[$wclass]?"Uninstall":"Install","RyeBread"); ?></button>
			</td>
			</tr>
			<?php
			$counter++;
		} 
	?>
	</table>
	</div>