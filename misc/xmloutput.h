#ifndef XML_OUTPUT_H
#define XML_OUTPUT_H

#include <iosfwd>


namespace Misc
{
    class Output_impl;
    class XMLoutput
        {
        public:
            XMLoutput(bool silent = false);
            ~XMLoutput();
            
            void push(const char *node_name);
            void pop();
            void add(std::string name, std::string text); //Oscar          
            void add(const char *node_name, const char *text = 0);
            void add(const char *node_name, int i);
            void add(const char *node_name, double i);
            void attr(std::string name, std::string text); //Oscar
            void attr(const char *attr_name, const char *attr_value);
            void attr(const char *attr_name, int attr_value);
            void attr(const char *attr_name, double attr_value);
            void file(const char *file_name);
            void stream(std::ostream &);
            void init();
            void exit();
            void isLib();
            std::string getCharP();
            std::string fixXML(std::string text); //Oscar
            std::string fixXML(const char*text); //Oscar
        private:
            Output_impl *impl;
        };
    
    std::string fixXML(std::string text); //Oscar
    std::string fixXML(const char*text); //Oscar
}


#endif /* XML_OUTPUT_H */
