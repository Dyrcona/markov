/*
 * Copyright © 2012 Jason J.A. Stephenson <jason@sigio.com>
 *
 * This file is part of markov.
 *
 * markov is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * markov is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with markov.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef MARKOV_CHAIN_HH_INCL
#define MARKOV_CHAIN_HH_INCL

#include <deque>
#include <vector>
#include <map>
#include <string>
#include <istream>
#include <ostream>

/*!
 * \brief Namespace for Markov chain implementaion.
 */
namespace markov {

/*!
 * \brief A class to implement a Markov chain text generator.
 *
 * \see http://en.wikipedia.org/wiki/Markov_chain
 *
 * \warning Several of the methods make use of library calls that are
 * not thread safe.
 */
class chain : public std::map<std::deque<std::string>,
															std::vector<std::string> > {
	
public:

	/*!
	 * \brief Define a type for the chain prefix.
	 */
	typedef std::deque<std::string> prefix;

	/*!
	 * \brief The constructor.
	 *
	 * \param len The length of the prefix used in the chain.
	 */
	chain(std::size_t len = 2) :
		std::map<std::deque<std::string>,
						 std::vector<std::string> >(),
		prefix_len(len) {};

	/*!
	 * \brief Add a string to the chain.
	 *
	 * \param s The std::string to add.
	 */
	void add(const std::string& s);

	/*!
	 * \brief Add strings from a input stream to the chain.
	 *
	 * \param in The std::istream to read strings from.
	 * \param resetprefix Whether or not to clear the current prefix
	 * before adding strings.
	 */
	void add(std::istream& in, bool resetprefix = false);

	/*!
	 * \brief Generate scrambled text from the chain starting with a
	 * given prefix.
	 *
	 * \warning This method is not thread safe.
	 *
	 * \param s Stream to write the scrambled text to.
	 * \param nwords The number of words to write.
	 * \param pref The prefix to start at.
	 */
	void generate(std::ostream& s, std::size_t nwords, prefix pref);

	/*!
	 * \brief Generate scrambled text from the chain starting with a
	 * random prefix.
	 *
	 * \warning This method is not thread safe.
	 *
	 * \param s Stream to write the scrambled text to.
	 * \param nwords The number of words to write.
	 */
	void generate(std::ostream& s, std::size_t nwords);

	/*!
	 * \brief Output the chain to a stream in a format that can easily
	 * be read back in.
	 *
	 * The current data of the instance is written to the stream in a
	 * simple text format.  There is one line per chain entry.  Each
	 * line consists of the prefix followed by the suffix chain.  The
	 * prefix is separated from the suffix by the sequence of space,
	 * colon, space.  The prefix members are separated from each other
	 * by a space, as are the suffix members.  An example output file,
	 * created with the default prefix length of 2, of Hamlet's famous
	 * soliloquy follows:
	 *
	 * \verbinclude hamlet.txt
	 *
	 * \param s The stream to write to.
	 */
	void write(std::ostream& s);

	/*!
	 * \brief Read a chain in from a stream that was previously written
	 * with the write method.
	 *
	 * \param s The stream to read from.
	 * \see write
	 */
	void read(std::istream& s);

	/*!
	 * \brief Return the value of the current_prefix member.
	 */
	prefix currentPrefix();

	/*!
	 * \brief Return a random prefix from the chain.
	 */
	prefix randomPrefix();

	/*!
	 * \brief Check if a chain instance has a prefix matching the
	 * argument.
	 *
	 * \param pref The prefix to check.
	 * \return True if the argument appears as a prefix in the instance,
	 * false if not.
	 */
	bool isValidPrefix(prefix pref);

	/*!
	 * \brief Return the value of the prefix length member.
	 */
	std::size_t prefixLength();

	/*!
	 * \brief Check if the class has seeded the random number generator.
	 *
	 * This method is generally used by the implementation methods of
	 * this class.  It is available if for some reason you want to know
	 * if the pseudo-random number generator has been seeded.
	 *
	 * \return True if the prng has been seeded, false if not.
	 */
	static bool isSeeded();

	/*!
	 * \brief Seed the class wide random number generator.
	 *
	 * Client code generally does not need to use this class method.
	 * The other members will call it if necessary.  It is there,
	 * however, if you do want to change the seed on the pseudo-random
	 * number generator for some reason.  Keep in mind that it doesn't
	 * let you pick the seed.
	 *
	 * Also, if the isSeeded static method would return true, this
	 * method will not actually do anything unless the force argument is
	 * true.  The default is false.
	 *
	 * \warning This method is not thread safe.
	 *
	 * \param force If true, reseed the generator, even if it has
	 * already been seeded.
	 */
	static void seed(bool force = false);

protected:

	/*!
	 * \brief Set the current prefix member to the argument.
	 *
	 * Client code generally has no business changing the current prefix
	 * value.  The public methods will change it as necessary.  However,
	 * a subclass has no way to change the current prefix value and may
	 * need to, particularly when reimplement the read method.  Since
	 * this method checks the validity of the prefix parameter, it
	 * should be used after the new prefix has been added to the
	 * instance.
	 *
	 * \param pref The new value for the current prefix.
	 * \return The value of current prefix after calling this method.
	 */
	prefix currentPrefix(prefix pref);

	/*!
	 * \brief Change the value of the object's prefix length member.
	 *
	 * This method should not be needed by client code.  Subclasses that
	 * reimplement the read method, perhaps to support a different file
	 * storage format, may need to call this method to set the prefix
	 * length.
	 *
	 * This method has the side effect of clearing the instance as well
	 * as the instance's current prefix value.  Therefore, it should be
	 * called before setting any values in a read.
	 *
	 * \param len The new length for the prefix length.
	 * \return The new length for the prefix length, if successful.
	 */
	std::size_t prefixLength(std::size_t len);

private:
	prefix current_prefix;
	std::size_t prefix_len;
	static bool is_seeded;
	bool parseLine(std::string line);

};

}

#endif // MARKOV_CHAIN_HH_INCL
