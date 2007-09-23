import sys, os
from optparse import OptionParser


class MakeDictParser:
		def __init__(self):
				self.dict_info = { 'full_name': '', 
                                                   'lang_from': '', 
                                                   'lang_to': '',
                                                   'type': '',
                                                   'description': ''
                                                   }
				self.f = None
				self.parser_info = { 'version': '',
                                                     'dict_name': '',
                                                     'format': ''
                                                     }

		def __del__(self):
				if self.f:
						self.f.close()

		def set_dict_info(self, key, data):
				self.dict_info[key]=data

		def begin(self):
				print "<meta_info><basename>"+self.dict_info["basename"]+"</basename></meta_info>"
				print "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n" \
						"<!DOCTYPE xdxf SYSTEM \"http://xdxf.sourceforge.net/xdxf_lousy.dtd\">\n"

		def end(self):
				print "</xdxf>"

		def info(self):
				print "<xdxf lang_from=\""+self.dict_info["lang_from"]+"\" lang_to=\""+self.dict_info["lang_to"]+"\" format=\"visual\">\n" \
						" <full_name>"+self.dict_info["full_name"]+"</full_name>\n" \
						" <description>"+self.dict_info["description"]+"</description>\n" \

		def abbrs_begin(self):
				print "<abbreviations>"

		def abbrs_end(self):
				print "</abbreviations>"
		def start(self):
				def input_format(option, opt, value, parser):
						print self.parser_info['format']
						sys.exit(0)				

				def store_value(option, opt_str, value, parser):
						setattr(parser.values, option.dest, value)

				parser = OptionParser(usage="%prog path/to/"+self.parser_info['dict_name'],
                                                      version="%prog "+self.parser_info['version'])
				parser.add_option("-i", "--input-format", action="callback", 
                                                  callback=input_format, 
                                                  help="output supported input format and exit")
				parser.add_option("--is-your-format",
                                                  action="callback", callback=store_value,
                                                  type="string", nargs=1, dest="is_my_format")
                                

				(options, args) = parser.parse_args()

				if options.is_my_format:
						if os.path.basename(options.is_my_format) ==\
                                                        self.parser_info['dict_name']:
								sys.exit(0)
						else:
								sys.exit(1)

				if len(args) != 1:
						parser.error("incorrect number of arguments")
                                self.input_file_name = args[0];
				self.f = open(args[0], 'r')

#md=MakeDictParser()
#md.parser_info['version']='0.1'
#md.parser_info['dict_name']='Mueller7GPL.koi'
#md.parser_info['format']='mueller7'
#md.start()
