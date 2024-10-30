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
?>
<div class="man_console">
	<h3><?php _e("Widget management","RyeBread"); ?></h3>
	<form method="post">
	<input type="hidden" name="rbaction" value="rbcreatewidget" />
	<div>
		<input type="edit" name="wname" value="" size="20" />
		<select name="wclass">
		<?php
			$wclasses = rb_installed_widget_classes();
			foreach ($wclasses as $wclass){
				?>
				<option value="<?php echo $wclass->id; ?>"><?php echo $wclass->classname; ?></option>
				<?php
			} 
		?>
		</select>
		<button onclick="submit();"><?php _e("Add","RyeBread"); ?></button>
	</div>
	</form>
	<script language="javascript">
		function flipConf(id){
		 if (document.getElementById("conf"+id).style.display == "none")
		 	document.getElementById("conf"+id).style.display = "block";
		 	else
		 	document.getElementById("conf"+id).style.display = "none";
		}
		
		function editWidget(id){
			newname = document.getElementById("wn_"+id).value; 
			jQuery.ajax({
				data: {action:"rb_admin_call",rbadminaction:"editwidget",widget:id,wname:newname},
				type: "POST",
				dataType: "script",
				url: "<?php echo get_option('siteurl')."/wp-admin/admin-ajax.php";  ?>"
				});
		} 
		
		function deleteWidget(id){
			document.getElementById("del_"+id).disabled = true;
			jQuery.ajax({
				data: {action:"rb_admin_call",rbadminaction:"delwidget",widget:id},
				type: "POST",
				dataType: "script",
				url: "<?php echo get_option('siteurl')."/wp-admin/admin-ajax.php";  ?>"
				});
		}
	</script>
	<?php
		$widgets = rb_widgets();
		$counter = 0;
		foreach ($widgets as $w){
				$widget = rb_widget(null,$w->id);
				?>
				<div id="w_<?php echo $w->id; ?>" class="widget">
				<div id="wt_<?php echo $w->id; ?>" class="widget_title">
				<table style="width:100%;">
					<tr onclick="javascript:flipConf(&quot;<?php echo $w->id; ?>&quot;);">
						<td>
						<input id="wn_<?php echo $w->id; ?>" type="edit" value="<?php echo $w->name; ?>" size="20" onclick="javascript:window.event.cancelBubble = true;" onchange="javascript:editWidget(&quot;<?php echo $w->id; ?>&quot;);" />:&nbsp;<?php echo $w->classname; ?>
						</td>
						<td style="text-align:right;">
						<button id="del_<?php echo $w->id; ?>" onclick="javascript:deleteWidget(&quot;<?php echo $w->id; ?>&quot;);window.event.cancelBubble = true;"><?php _e("Delete","RyeBread"); ?></button>
						</td>
					</tr>
				</table>
				</div>
				<div id="conf<?php echo $w->id; ?>" class="widget_conf" style="display:none;">
					<?php
					$widget->ConfigPage();
					?>
				</div>
				</div>
				<?php
				$counter++;
			} 
	?>
</div>