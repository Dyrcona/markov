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
#include <config.h>
#include <chain.hh>
#include <cstdlib>

#ifdef HAVE_RANDOM_DEVICE
#include <fstream>
#else
#include <ctime>
#endif

namespace markov {

bool chain::is_seeded = false;

static std::size_t getSeed(void)
{
  std::size_t seed = 0;
#ifdef HAVE_RANDOM_DEVICE
  std::ifstream in(RANDOM_DEVICE, std::ios::in);
  in.read((char *)&seed, sizeof(seed));
  in.close();
#else
  (void)std::time((std::time_t *)&seed);
#endif
  return seed;
}

void chain::add(const std::string& s) {
  if (this->current_prefix.size() == this->prefix_len) {
    (*this)[this->current_prefix].push_back(s);
    this->current_prefix.pop_front();
  }
  this->current_prefix.push_back(s);
}

void chain::add(std::istream& in, bool resetprefix) {
  std::string buf;
  if (resetprefix)
    this->current_prefix.clear();
  while(in >> buf)
    this->add(buf);
}

void chain::generate(std::ostream& s, std::size_t nwords, prefix pref) {
  if (this->isValidPrefix(pref))
    this->current_prefix = pref;
  else
    this->current_prefix = this->randomPrefix();

  if (!chain::is_seeded)
    chain::seed();

  std::size_t i;
  for (i = 0; i < this->prefix_len; i++)
    s << this->current_prefix.at(i) << ' ';

  for (; i < nwords; i++) {
    std::vector<std::string>& suf = (*this)[current_prefix];
    const std::string& w = suf[random() % suf.size()];
    s << w << ' ';
    // To avoid a SIGFPE when we get the last entry from the original
    // input:
    pref = this->current_prefix;
    pref.pop_front();
    pref.push_back(w);
    if (this->isValidPrefix(pref))
      this->current_prefix = pref;
    else
      break;
  }

  s << std::endl;
}

void chain::generate(std::ostream& s, std::size_t nwords) {
  prefix start = this->randomPrefix();
  this->generate(s, nwords, start);
}

void chain::write(std::ostream& s) {
  for (const_iterator it = this->begin(); it != this->end(); it++) {
    prefix pref = it->first;
    std::vector<std::string> suf = it->second;

    for (prefix::const_iterator pit = pref.begin(); pit != pref.end(); pit++)
      s << *pit << ' ';
    s << ": ";

    std::vector<std::string>::const_iterator vit = suf.begin();
    while (vit != suf.end()) {
      s << *vit++;
      if (vit != suf.end())
        s << ' ';
      else
        s << std::endl;
    }
  }
}

void chain::read(std::istream &s) {
  this->clear();
  this->current_prefix.clear();
  this->prefix_len = 0;
  while (s.good()) {
    std::string line;
    std::getline(s, line);
    if (s.good())
      this->parseLine(line);
  }
}

bool chain::parseLine(std::string line) {
  bool success = false;
  std::size_t cpos = line.find(" : ");
  if (cpos != std::string::npos) {
    prefix temp;
    std::string pstr = line.substr(0, cpos);
    std::size_t spos = 0;

    do {
      spos = pstr.find(' ');
      temp.push_back(pstr.substr(0, spos));
      if (spos != std::string::npos)
        pstr = pstr.substr(spos + 1);
    } while (spos != std::string::npos);

    if (this->prefix_len == 0)
      this->prefix_len = temp.size();

    if (temp.size() == this->prefix_len) {
      this->current_prefix = temp;
      pstr = line.substr(cpos + 3);

      do {
        spos = pstr.find(' ');
        (*this)[temp].push_back(pstr.substr(0, spos));
        if (spos != std::string::npos)
          pstr = pstr.substr(spos + 1);
      } while (spos != std::string::npos);

      success = true;
    }
  }
  return success;
}


chain::prefix chain::currentPrefix() {
  return this->current_prefix;
}

chain::prefix chain::currentPrefix(prefix pref) {
  if (this->isValidPrefix(pref))
    this->current_prefix = pref;
  return this->current_prefix;
}

chain::prefix chain::randomPrefix() {
  prefix pref;

  if (!chain::is_seeded)
    chain::seed();

  std::size_t i = random() % this->size();
  std::size_t j = 0;
  for (const_iterator it = this->begin(); it != this->end() && j <= i;
       it++, j++)
    pref = it->first;

  return pref;
}

bool chain::isValidPrefix(prefix pref) {
  bool isValid = true;
  iterator it = this->find(pref);
  if (it == this->end())
    isValid = false;

  return isValid;
}

std::size_t chain::prefixLength() {
  return this->prefix_len;
}

std::size_t chain::prefixLength(std::size_t len) {
  this->clear();
  this->current_prefix.clear();
  this->prefix_len = len;
  return this->prefix_len;
}

bool chain::isSeeded() {
  return chain::is_seeded;
}

void chain::seed(bool force) {
  if (force || !chain::is_seeded) {
    srandom(getSeed());
    chain::is_seeded = true;
  }
}

}
