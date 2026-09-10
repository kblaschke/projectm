// ConfigFile.h
// Class for reading named values from configuration files
// Richard J. Wagner  v2.1  24 May 2004  wagnerr@umich.edu

// Copyright (c) 2004 Richard J. Wagner
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

// Typical usage
// -------------
//
// Given a configuration file "settings.inp":
//   atoms  = 25
//   length = 8.0  # nanometers
//   name = Reece Surcher
//
// Named values are read in various ways, with or without default values:
//   ConfigFile config( "settings.inp" );
//   int atoms = config.read<int>( "atoms" );
//   double length = config.read( "length", 10.0 );
//   string author, title;
//   config.readInto( author, "name" );
//   config.readInto( title, "title", string("Untitled") );
//
// See file example.cpp for more examples.

#pragma once

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <utility>

class ConfigFile
{
public:
    // Exception types
    class FileNotFound : public std::exception
    {
    public:
        FileNotFound(std::string missingFile = std::string())
            : filename(std::move(missingFile))
        {
        }

        std::string filename;
    };

    class KeyNotFound : public std::exception
    { // thrown only by T read(key) variant of read()
    public:
        explicit KeyNotFound(std::string missingKey = std::string())
            : key(std::move(missingKey))
        {
        }

        std::string key;
    };

    // Methods

    /**
     * Construct a ConfigFile without a file; empty
     */
    ConfigFile();

    /**
     * Construct a ConfigFile, getting keys and values from given file
     * @param filename The configuration file path
     * @param delimiter The key/value delimiter string (default: =)
     * @param comment The comment start string (default: #)
     * @param sentry A sentry string signifying the end of the configuration file (default: EndConfigFile)
     */
    explicit ConfigFile(const std::string& filename,
                        std::string delimiter = "=",
                        std::string comment = "#",
                        std::string sentry = "EndConfigFile");

    /**
     * Search for key and read value or optional default value
     */
    template<class T>
    T Read(const std::string& key) const; // call as read<T>

    /**
     * Search for key and read value or optional default value
     */
    template<class T>
    T Read(const std::string& key, const T& value) const;
    template<class T>
    bool ReadInto(T& var, const std::string& key) const;
    template<class T>
    bool ReadInto(T& var, const std::string& key, const T& value) const;

    // Modify keys and values
    template<class T>
    void Add(std::string key, const T& value);
    void Remove(const std::string& key);

    // Check whether key exists in configuration
    [[nodiscard]] auto KeyExists(const std::string& key) const -> bool;

    // Check or change configuration syntax
    [[nodiscard]] auto GetDelimiter() const -> std::string
    { return m_delimiter; }
    [[nodiscard]] auto GetComment() const -> std::string
    { return m_comment; }
    [[nodiscard]] auto GetSentry() const -> std::string
    { return m_sentry; }
    auto SetDelimiter(const std::string& delimiter) -> std::string
    {
        std::string old = m_delimiter;
        m_delimiter = delimiter;
        return old;
    }
    auto SetComment(const std::string& comment) -> std::string
    {
        std::string old = m_comment;
        m_comment = comment;
        return old;
    }

    // Write or read configuration
    friend auto operator<<(std::ostream& outputStream, const ConfigFile& configFile) -> std::ostream&;
    friend auto operator>>(std::istream& inputStream, ConfigFile& configFile) -> std::istream&;

protected:
    template<class T>
    static auto ToString(const T& value) -> std::string;
    template<class T>
    static auto FromString(const std::string& value) -> T;
    static void Trim(std::string& value);

    std::string m_delimiter;                       //!< separator between key and value
    std::string m_comment;                         //!< separator between value and comments
    std::string m_sentry;                          //!< optional string to signal end of file
    std::map<std::string, std::string> m_contents; //!< extracted keys and values
};


/* static */
template<class T>
auto ConfigFile::ToString(const T& value) -> std::string
{
    // Convert from a T to a string
    // Type T must support << operator
    std::ostringstream ost;
    ost << value;
    return ost.str();
}


/* static */
template<class T>
auto ConfigFile::FromString(const std::string& value) -> T
{
    // Convert from a string to a T
    // Type T must support >> operator
    T t;
    std::istringstream ist(value);
    ist >> t;
    return t;
}


/* static */
template<>
inline auto ConfigFile::FromString<std::string>(const std::string& value) -> std::string
{
    // Convert from a string to a string
    // In other words, do nothing
    return value;
}


/* static */
template<>
inline auto ConfigFile::FromString<bool>(const std::string& value) -> bool
{
    // Convert from a string to a bool
    // Interpret "false", "F", "no", "n", "0" as false
    // Interpret "true", "T", "yes", "y", "1", "-1", or anything else as true
    std::string stringUppercase = value;
    for (char& character : stringUppercase)
    {
        character = static_cast<char>(toupper(character)); // make string all caps
    }

    return !(stringUppercase == std::string("FALSE") || stringUppercase == std::string("F") ||
             stringUppercase == std::string("NO") || stringUppercase == std::string("N") ||
             stringUppercase == std::string("0") || stringUppercase == std::string("NONE"));
}


template<class T>
T ConfigFile::Read(const std::string& key) const
{
    // Read the value corresponding to key
    const auto p = m_contents.find(key);
    if (p == m_contents.end())
    {
        throw KeyNotFound(key);
    }
    return FromString<T>(p->second);
}


template<class T>
T ConfigFile::Read(const std::string& key, const T& value) const
{
    // Return the value corresponding to key or given default value
    // if key is not found
    const auto p = m_contents.find(key);
    if (p == m_contents.end())
    {
        return value;
    }
    return FromString<T>(p->second);
}


template<class T>
bool ConfigFile::ReadInto(T& var, const std::string& key) const
{
    // Get the value corresponding to key and store in var
    // Return true if key is found
    // Otherwise leave var untouched
    const auto p = m_contents.find(key);
    const bool found = (p != m_contents.end());
    if (found)
    {
        var = FromString<T>(p->second);
    }
    return found;
}


template<class T>
bool ConfigFile::ReadInto(T& var, const std::string& key, const T& value) const
{
    // Get the value corresponding to key and store in var
    // Return true if key is found
    // Otherwise set var to given default
    const auto p = m_contents.find(key);
    const bool found = p != m_contents.end();
    if (found)
    {
        var = FromString<T>(p->second);
    }
    else
    {
        var = value;
    }
    return found;
}


template<class T>
void ConfigFile::Add(std::string key, const T& value)
{
    // Add a key with given value
    std::string stringValue = ToString(value);
    Trim(key);
    Trim(stringValue);
    m_contents[key] = stringValue;
}

// Release notes:
// v1.0  21 May 1999
//   + First release
//   + Template read() access only through non-member readConfigFile()
//   + ConfigurationFileBool is only built-in helper class
//
// v2.0  3 May 2002
//   + Shortened name from ConfigurationFile to ConfigFile
//   + Implemented template member functions
//   + Changed default comment separator from % to #
//   + Enabled reading of multiple-line values
//
// v2.1  24 May 2004
//   + Made template specializations inline to avoid compiler-dependent linkage
//   + Allowed comments within multiple-line values
//   + Enabled blank line termination for multiple-line values
//   + Added optional sentry to detect end of configuration file
//   + Rewrote messy trimWhitespace() function as elegant trim()
