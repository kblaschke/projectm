#include "ConfigFile.h"

#include <utility>

ConfigFile::ConfigFile()
    : m_delimiter(std::string(1, '='))
    , m_comment(std::string(1, '#'))
{
}

ConfigFile::ConfigFile(const std::string& filename, std::string delimiter,
                       std::string comment, std::string sentry)
    : m_delimiter(std::move(delimiter))
    , m_comment(std::move(comment))
    , m_sentry(std::move(sentry))
{
    std::ifstream in(filename.c_str());

    if (!in)
        throw FileNotFound(filename);

    in >> (*this);
}


void ConfigFile::Remove(const std::string& key)
{
    // Remove key and its value
    m_contents.erase(m_contents.find(key));
}


bool ConfigFile::KeyExists(const std::string& key) const
{
    // Indicate whether key is found
    const auto p = m_contents.find(key);
    return p != m_contents.end();
}


/* static */
void ConfigFile::Trim(std::string& value)
{
    // Remove leading and trailing whitespace
    static constexpr char whitespace[] = " \n\t\v\r\f";
    value.erase(0, value.find_first_not_of(whitespace));
    value.erase(value.find_last_not_of(whitespace) + 1U);
}


std::ostream& operator<<(std::ostream& outputStream, const ConfigFile& configFile)
{
    // Save a ConfigFile to os
    for (const auto& line : configFile.m_contents)
    {
        outputStream << line.first << " " << configFile.m_delimiter << " ";
        outputStream << line.second << std::endl;
    }
    return outputStream;
}


std::istream& operator>>(std::istream& inputStream, ConfigFile& configFile)
{
    // Load a ConfigFile from is
    // Read in keys and values, keeping internal whitespace
    using pos = std::string::size_type;

    const std::string& delim = configFile.m_delimiter; // separator
    const std::string& comm = configFile.m_comment;    // comment
    const std::string& sentry = configFile.m_sentry;   // end of file sentry
    const pos skip = delim.length();                   // length of separator

    std::string nextline; // might need to read ahead to see where value ends

    while (inputStream || !nextline.empty())
    {
        // Read an entire line at a time
        std::string line;
        if (!nextline.empty())
        {
            line = nextline; // we read ahead; use it now
            nextline = "";
        }
        else
        {
            std::getline(inputStream, line);
        }

        // Ignore comments
        line = line.substr(0, line.find(comm));

        // Check for end of file sentry
        if (!sentry.empty() && line.find(sentry) != std::string::npos)
            return inputStream;

        // Parse the line if it contains a delimiter
        pos delimPos = line.find(delim);
        if (delimPos < std::string::npos)
        {
            // Extract the key
            std::string key = line.substr(0, delimPos);
            line.replace(0, delimPos + skip, "");

            // See if value continues on the next line
            // Stop at blank line, next line with a key, end of stream,
            // or end of file sentry
            bool terminate = false;
            while (!terminate && inputStream)
            {
                std::getline(inputStream, nextline);
                terminate = true;

                std::string nlcopy = nextline;
                ConfigFile::Trim(nlcopy);
                if (nlcopy.empty())
                    continue;

                nextline = nextline.substr(0, nextline.find(comm));
                if (nextline.find(delim) != std::string::npos)
                    continue;
                if (!sentry.empty() && nextline.find(sentry) != std::string::npos)
                    continue;

                nlcopy = nextline;
                ConfigFile::Trim(nlcopy);
                if (!nlcopy.empty())
                    line += '\n';
                line += nextline;
                terminate = false;
            }

            // Store key and value
            ConfigFile::Trim(key);
            ConfigFile::Trim(line);
            configFile.m_contents[key] = line; // overwrites if key is repeated
        }
    }

    return inputStream;
}
