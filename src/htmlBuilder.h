#ifndef HTML_BUILDER_H
#define HTML_BUILDER_H
#include <Arduino.h>
class stringBuilder {
private:
	String _start = "";
public:
	stringBuilder();
	void appendLine(String s);
	void write_toSpiff();
};
void HTMLBuild_socket();
#endif // HTML_BUILDER_H
