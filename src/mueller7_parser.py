#!/usr/bin/env python
# -*- coding: utf-8 -*-


import sys, codecs, re, xml.sax.saxutils, os
from optparse import OptionParser
import mdparser


decoding_map = codecs.make_identity_dict(range(256))
decoding_map.update({				
				ord('Q'): 230, # "a" from "man"
				ord('W'): 695 , # "w"
				ord('A'): 593, # "a" from "past"
				249: ord(':'), # ":"
				171: 601, # "e" from "her"
				ord('E'): 603, # "e" first from diphthong in "care"
				141: 596, # "o" from "wash"
				195: 652, # "a" from "son"
				ord('I'): 618, # "i" from "ink"
				200: 712, # "'"
				199: 716, # ","
				ord('H'): 688, # "h"
				ord('Z'): 658, # "z"
				ord('N'): 331, # "ng"
				ord('S'): 643, # "sh"
				ord('D'): 240, # "th" with voice
				ord('T'): 952, # "th"				
})

encoding_map = codecs.make_encoding_map(decoding_map)

class Codec(codecs.Codec):

    def encode(self,input,errors='strict'):

        return codecs.charmap_encode(input,errors,encoding_map)

    def decode(self,input,errors='strict'):

        return codecs.charmap_decode(input,errors,decoding_map)

class StreamWriter(Codec,codecs.StreamWriter):
    pass

class StreamReader(Codec,codecs.StreamReader):
    pass

def find_sil_codec(name):
		if (name=='sil'):
				return (Codec().encode,Codec().decode,StreamReader,StreamWriter)
		else:
				return none

codecs.register(find_sil_codec)

def decode_from_koi(str):
		return str.decode("koi8-r").encode("utf-8")

parser = mdparser.MakeDictParser()

parser.parser_info['version']='0.2'
parser.parser_info['dict_name']='Mueller7GPL.koi'
parser.parser_info['format']='mueller7'
parser.start()

description=decode_from_koi(parser.f.readline())+\
		decode_from_koi(parser.f.readline())+\
		decode_from_koi(parser.f.readline()).rstrip()

if parser.input_file_name.endswith('Mueller7accentGPL.koi'):
    parser.set_dict_info("basename", "Mueller7accentGPL")
else:
    parser.set_dict_info("basename", "Mueller7GPL")

parser.begin()

if parser.input_file_name.endswith('Mueller7accentGPL.koi'):
    parser.set_dict_info('full_name', 'Англо-русский словарь Мюллера, 7-я ред., с ударениями')
else:
    parser.set_dict_info('full_name', 'Англо-русский словарь Мюллера, 7-я ред.')

parser.set_dict_info('lang_from', 'ENG')
parser.set_dict_info('lang_to', 'RUS')
parser.set_dict_info('description', xml.sax.saxutils.escape(description))
parser.info()
parser.abbrs_begin()

art_beg='<abr_def>'
art_end='</abr_def>'
key_end='</k>'
data_beg='<v>'
data_end='</v>'
read_abbr = 1
abbr_list={}
word_pat=re.compile("([\w\.-]+)", re.UNICODE)
ind_pat=re.compile("(?P<ind>\w+&gt;)", re.UNICODE)
ind_pat2=re.compile("(?P<ind>\d+\.)", re.UNICODE)
ex_pat=re.compile("(?P<exm>\s+[a-zA-Z0-9\.']+[a-zA-Z0-9\.'\s]+)", re.UNICODE)
#semicolon_pat=re.compile(";", re.UNICODE)

for line in parser.f.readlines():

		esc=re.compile('\[(.*?)\]')
		dosenc=esc.split(line) 
		i=-1
		res=''
		for sub in dosenc:
				i=i+1
				beg_tr=''
				if i==1:
					end_tr='</tr>\n'
				else:
					end_tr='</tr>'

				if (i % 2):
						res=res+'<tr>'+xml.sax.saxutils.escape(sub.decode('sil'))+end_tr
				else:
						res=res+xml.sax.saxutils.escape(sub.decode('koi8-r'))

		#		rez=''.join([x.decode('koi8-r')+decode_from_sil(y) for (x,y) in zip(dosenc,specenc)])
		#
		#key=decode_from_koi(key)
		key, data=res.split("  ")
		data=data.rstrip()
		new_data=''	
		if read_abbr==1:
			abbr_list[unicode(key)]=u'<abr>'+unicode(key)+u'</abr>'
		else:
			word_list=word_pat.split(data)
			for w in word_list:
				if w in abbr_list:
					new_data = new_data+abbr_list[w]
				else:
					new_data = new_data+w
			data=new_data
			data=ind_pat.sub("\n<b>\g<ind></b>", data)
			data=ind_pat2.sub("\n<b>\g<ind></b>", data)
			data=ex_pat.sub("<ex>\g<exm></ex>", data)

		print art_beg+'<k>' + xml.sax.saxutils.escape(key.encode('utf-8')) +\
				key_end + data_beg + data.encode('utf-8') + data_end + art_end
		if key==u'_яп.':
				parser.abbrs_end()
				read_abbr=0
				art_beg="<ar>"
				art_end="</ar>"
				data_beg=""
				data_end=""
				key_end='</k>\n'

parser.end()
