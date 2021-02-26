
#include <Arduino.h>
#include <adri_soft_logger.h>
#include <adri_tools_v2.h>
#include <adri_tools_v2_serialMenu.h>

#include "factory_reset.h"

unsigned long ptr=0;



//////////////////////////////////////// FILE SYSTEM ///////////////////////////////////////////
	void trim(String * s) {
		while (s->indexOf("\n")>=0) s->replace("\n","");
		while (s->indexOf("\r")>=0) s->replace("\r","");
	}

	void string_hex_dump(String s) {
		char h[5];
		String dump="  -  [";
		for (int i=0; i<s.length(); i++) {
			sprintf(h," %02x",s.charAt(i));
			dump+=String(h);
		}
		dump+="]";
		Serial.println(s+dump);
	}

	void string_trim_char(String * s, char c) {
		while (s->indexOf(c)!=-1) s->replace(String(c),"");
	}

	void string_trim(String * s) {
		string_trim_char(s,13);
	}

	boolean str_equ(const char * source, const String cmp) {
		char c[80];
		fssprintf(c,"%s",cmp.c_str());
		return (strcmp(source,c)==0);
	}




	boolean file_debug			= true;
	boolean file_line_debug     = false;
	boolean file_error_debug	= false;
	int open_files 				= 0;

	void file_trace(File * f, String operation) {
		fsprintf(">>>file %03d op %-30s %s\n",  open_files, operation.c_str(), f->name());
	}
	void file_trace_by_name(String file_name, String operation) {
		fsprintf(">>>file %03d op %-30s %s\n", open_files, operation.c_str(), file_name.c_str() );
	}

	boolean file_open(String file_name, String mode, File * f) {
		LittleFS.begin();
		open_files++;
		String op="open "+mode;
		if (file_debug) file_trace_by_name(file_name,op);
		*f=LittleFS.open(file_name, mode.c_str());
		if (!*f) {
			if (file_debug) file_trace_by_name(file_name, op+" error");
			return false;
		} else return true;
	}

	void file_close(File * f) {
		String s=f->name();
		f->close();
		open_files--;
		if (file_debug) file_trace_by_name(s,F("close"));
	}

	String		file_read_string_until(File * f, char c) {
		String op="read string until char("+String(int(c))+")";
		if (file_line_debug) file_trace(f, op);
		int		pos1	= f->position();
		String	s		= f->readStringUntil(c);
				s.replace(String(c),""); //$$$16022019
		int		pos2	= f->position();
		if (pos2==pos1) {									// when the file pointer does not move, we have a file system error
			f->seek(f->size(),SeekSet);
			s= ">>>file error<<<";
		} else {
			string_trim(&s);
		}
		return s;
	}

	String		file_read_string(File * f) {
		return file_read_string_until(f, '\n');
	}

	boolean	file_exists(String file_name) {
		if (file_debug) file_trace_by_name(file_name, "file exists");
		LittleFS.begin();
		boolean e=LittleFS.exists(file_name);
		return e;
	}

	boolean file_writeln_string(File * f, String * s) {
		if (file_line_debug) file_trace(f, "file writeln string");
		boolean yes = f->println(*s);
		return yes;
	}

	boolean file_eof (File * f) {
		return (f->position()>=f->size());
	}

	boolean file_eof(File f) {
		//unsigned int p=f.position();
		//unsigned int s=f.size();
		//if (p>=s) return true; else return false;
		return (f.position()>=f.size());
	}

	boolean file_remove(String file_name) {
		if (file_debug) file_trace_by_name(file_name,F("remove"));
		if (!LittleFS.remove(file_name)) {
			file_trace_by_name(file_name, "remove error");
			return false;
		}
		else return true;
	}

	boolean file_rename(String old_name, String new_name) {
		if (file_debug) {
			file_trace_by_name(old_name,		F("rename from"));
			file_trace_by_name(new_name,		F("rename to")	);
		}
		if (!LittleFS.rename(old_name, new_name)) {
			file_trace_by_name(old_name, "rename error");
			return false;
		}
		else return true;
	}











/***********************************  FACTORY RESET MANAGEMENT *******************************/



void write_bytes(String file_name, unsigned long start, unsigned long end) {
	//fsprintf(" - writing file '%s'\n", file_name.c_str());
	fsprintf("  %-35s %8u bytes\n", file_name.c_str(), end-start);
	char c;
	String s=""; int nbline=1;
	File f; file_open("/"+file_name,"w",&f);
	for (unsigned long i=start; i<end; i++)   {
		c=pgm_read_byte_near(factory_defaults_binary+i);
		//s+=c;
		if (c==char(10)) {
			//s.replace("\n","");
			//s.replace("\r","");
			//Serial.printf("                 %8d %8d %s\n",i,nbline,s.c_str()); s="";
			nbline++;
		}
		f.print(c);
		yield();
	}
	file_close(&f);
}

//void factory_reset() {
//	ptr=0;
//	unsigned long start, end;
//	String file_name,s;
//	int file_nbr=0;
//	fsprintln("factory reset start, restoring files:");
//	while (file_name!="EOF") {									// we scan the factory_defaults descriptor list
//		int i,j;
//		s = factory_defaults_descriptor[file_nbr++];			// we extract the current file descriptor
//		int a=s.indexOf(" ");
//		file_name=s.substring(0,a);								// we build the file name
//		i=file_name.length()+1;
//		while (s.charAt(i)==' ') i++;							// we seek for start of number
//		j=i;
//		while (s.charAt(j)!=' ') j++;							// we seek for end of number
//		start=s.substring(i,j).toInt();							// we build the start index
//
//		//Serial.printf(" start i:%d j:%d start:%d ",i,j,start);
//		i=j;
//		while (s.charAt(i)==' ') i++;							// we seek for start of number
//		j=i;
//		while (s.charAt(j)!=' ') j++;							// we seek for end of number
//		end=s.substring(i,j).toInt()-1;							// we build the end index
//
//		if (file_name!="EOF") write_bytes(file_name, start, end);
//	}
//	fsprintln("device returned to factory settings");
//	//file_remove("bitmap_index.txt");
//	bitmap_create_index();
//}

/*
 * provides address (start) and length(length) of the file requested
 * returns true if file was found, or false
 */
boolean static_file_content(String * search_name, const unsigned char ** kstart, int * klength) {
	ptr=0;
	unsigned long start, end;
	String file_name,s;
	int file_nbr=0;
	if (search_name->charAt(0)=='/') *search_name=search_name->substring(1);
	while (file_name!="EOF") {									// we scan the factory_defaults descriptor list
		int i,j;
		s = factory_defaults_descriptor[file_nbr++];			// we extract the current file descriptor
		int a=s.indexOf(" ");
		file_name=s.substring(0,a);								// we build the file name
		i=file_name.length()+1;
		while (s.charAt(i)==' ') i++;							// we seek for start of number
		j=i;
		while (s.charAt(j)!=' ') j++;							// we seek for end of number
		start=s.substring(i,j).toInt();							// we build the start index
		i=j;
		while (s.charAt(i)==' ') i++;							// we seek for start of number
		j=i;
		while (s.charAt(j)!=' ') j++;							// we seek for end of number
		end=s.substring(i,j).toInt()-1;							// we build the end index
		if (file_name==*search_name) {
			*kstart=start+factory_defaults_binary;
			*klength=end-start;
			return true;
		}
	}
	return false;
}

void restore_spiffs_factory_files() {
	ptr=0;
	unsigned long start, end;
	String file_name,s;
	int file_nbr=0;
	fsprintf("factory reset start, restoring files:\n");
	while (file_name!="EOF") {									// we scan the factory_defaults descriptor list
		int i,j;
		s = factory_defaults_descriptor[file_nbr++];			// we extract the current file descriptor
		int a=s.indexOf(" ");
		file_name=s.substring(0,a);								// we build the file name
		i=file_name.length()+1;
		while (s.charAt(i)==' ') i++;							// we seek for start of number
		j=i;
		while (s.charAt(j)!=' ') j++;							// we seek for end of number
		start=s.substring(i,j).toInt();							// we build the start index
		i=j;
		while (s.charAt(i)==' ') i++;							// we seek for start of number
		j=i;
		while (s.charAt(j)!=' ') j++;							// we seek for end of number
		end=s.substring(i,j).toInt()-1;							// we build the end index
		#ifndef SPIFFS_DEBUG
			if ((file_name!="EOF") && (file_name.charAt(0)!='_')) 	write_bytes(file_name, start, end);
		#else
			if (file_name!="EOF") 									write_bytes(file_name, start, end);
		#endif
	}
	fsprintf("device returned to factory settings\n");
	//file_remove("bitmap_index.txt");
	// bitmap_create_index();
}
