#include "htmlBuilder.h"
    #include <LittleFS.h>

stringBuilder::stringBuilder(){

}
void stringBuilder::appendLine(String s){
    s.replace("'", "&apos;");
    s.replace("\"", "&quot;");
    _start += s + "\n";
}
void stringBuilder::write_toSpiff(){
    File file = LittleFS.open("test.js", "w");
    if (file) {
        file.print(_start);
        file.close(); 
    }     
}
void HTMLBuild_socket()
{

}
