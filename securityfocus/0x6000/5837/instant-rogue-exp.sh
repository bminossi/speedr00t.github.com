--- myng/dev/myarticles.php.old	Fri Sep 27 00:00:58 2002
+++ myng/dev/myarticles.php	Fri Sep 27 00:01:24 2002
@@ -126,7 +126,7 @@
                 $t->set_var("date",$date);
                 $t->set_var("id_article",$db->Record['id_article']);
                 $t->set_var("sender",$db2->Record['name']);
-                $t->set_var("subject",$db2->Record['subject']);
+                $t->set_var("subject",htmlspecialchars($db2->Record['subject']));
 
                 $group_url = "tree.php?group_name=".rawurlencode(real2table($db->Record['group_name']))."&begin=0&server=".rawurlencode($db->Record['server']);
                 $t->set_var("group_url",$group_url);
--- myng/dev/search.php.old	Fri Sep 27 00:03:14 2002
+++ myng/dev/search.php	Fri Sep 27 00:03:56 2002
@@ -97,7 +97,7 @@
                         $t->set_var("date",$date);
                         $article_url = "article.php?id_article=".rawurlencode($db->Record['id'])."&group_name=".$db->Record['newsgroup'];
                         $t->set_var("article_url",$article_url);
-                        $t->set_var("subject",$db->Record['subject']);
+                        $t->set_var("subject",htmlspecialchars($db->Record['subject']));
                         $t->set_var("sender",$db->Record['name']);
                         $t->set_var("group",$db->Record['newsgroup']);
                         $t->set_var("readings",$db->Record['num_readings']);
@@ -173,7 +173,7 @@
                                 $t->set_var("date",$date);
                                 $article_url = "article.php?id_article=".rawurlencode($db->Record['id'])."&group_name=".$db->Record['newsgroup'];
                                 $t->set_var("article_url",$article_url);
-                                $t->set_var("subject",$db->Record['subject']);
+                                $t->set_var("subject",htmlspecialchars($db->Record['subject']));
                                 $t->set_var("sender",$db->Record['name']);
                                 $t->set_var("group",$db->Record['newsgroup']);
                                 $t->set_var("readings",$db->Record['num_readings']);
--- myng/dev/stats.php.old	Fri Sep 27 00:05:14 2002
+++ myng/dev/stats.php	Fri Sep 27 00:06:40 2002
@@ -205,7 +205,7 @@
         while($db2->next_record()){
 
                 $num_replies[$j][0] = $db2->Record[1];
-                $num_replies[$j][1] = $db2->Record['subject'];
+                $num_replies[$j][1] = htmlspecialchars($db2->Record['subject']);
                 $j ++;
 
         }
@@ -246,7 +246,7 @@
         $db2->query($consulta2);
         while($db2->next_record()){
                 $num_readings[$j][0] = $db2->Record['num_readings'];
-                $num_readings[$j][1] = $db2->Record['subject'];
+                $num_readings[$j][1] = htmlspecialchars($db2->Record['subject']);
                 $j ++;
         }
 
--- myng/dev/lib/standard.lib.php.old	Thu Sep 26 23:54:04 2002
+++ myng/dev/lib/standard.lib.php	Thu Sep 26 23:58:13 2002
@@ -577,7 +577,7 @@
 
                         $li_image = "li".$li_number.$color.".gif";
 
-                        $linea2 = $linea."<img src=images/".$li_image." width=5 height=5>&nbsp;"."<a class=text href=article.php?id_article=".rawurlencode($db->Record['id'])."&group_name=".rawurlencode($group_name).">".$db->Record['subject']."</a>";
+                        $linea2 = $linea."<img src=images/".$li_image." width=5 height=5>&nbsp;"."<a class=text href=article.php?id_article=".rawurlencode($db->Record['id'])."&group_name=".rawurlencode($group_name).">".htmlspecialchars($db->Record['subject'])."</a>";
 
                         $reply_url = "post.php?type=reply&id=".$db->Record['number']."&group=".$group_name;
                         //echo $reply_url;
@@ -674,7 +674,7 @@
         $db->query($consulta);
         $db->next_record();
 
-        $url_subject = "<a class=text href=article.php?id_article=".rawurlencode($db->Record['id'])."&group_name=".$group_name.">".$db->Record['subject']."</a>";
+        $url_subject = "<a class=text href=article.php?id_article=".rawurlencode($db->Record['id'])."&group_name=".$group_name.">".htmlspecialchars($db->Record['subject'])."</a>";
         $t->set_var("p_subject",$url_subject);
         $c->username = $db->Record['username'];
         $c->email = $db->Record['from_header'];
