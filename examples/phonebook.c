// The MIT License (MIT)
// Copyright (c) 2018 Maksim Andrianov
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

// Program to emulates the phone book.

#include <stdio.h>
#include <stc/cmap.h>
#include <stc/cstr.h>

cdef_cmap_str();

void print_phone_book(cmap_str phone_book)
{
  c_foreach (i, cmap_str, phone_book)
    printf("%s\t- %s\n", i.get->key.str, i.get->value.str);
}

int main(int argc, char **argv)
{
  bool erased;
  cmap_str phone_book = cmap_ini;
  c_push(&phone_book, cmap_str, {
    {"Lilia Friedman", "(892) 670-4739"},
    {"Tariq Beltran", "(489) 600-7575"},
    {"Laiba Juarez", "(303) 885-5692"},
    {"Elliott Mooney", "(945) 616-4482"},
  });

  printf("Phone book:\n");
  print_phone_book(phone_book);

  c_try_emplace(&phone_book, cmap_str, "Zak Byers", cstr("(551) 396-1880"));
  c_try_emplace(&phone_book, cmap_str, "Zak Byers", cstr("(551) 396-1990"));

  printf("\nPhone book after adding Zak Byers:\n");
  print_phone_book(phone_book);

  if (cmap_str_find(&phone_book, "Tariq Beltran") != NULL)
    printf("\nTariq Beltran is in phone book\n");

  erased = cmap_str_erase(&phone_book, "Tariq Beltran");
  erased = cmap_str_erase(&phone_book, "Elliott Mooney");

  printf("\nPhone book after erasing Tariq and Elliott:\n");
  print_phone_book(phone_book);

  cmap_str_insert_or_assign(&phone_book, "Zak Byers", "(555) 396-188");

  printf("\nPhone book after update phone of Zak Byers:\n");
  print_phone_book(phone_book);

  cmap_str_destroy(&phone_book);
  puts("done");
}