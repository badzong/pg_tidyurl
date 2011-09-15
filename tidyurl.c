#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <postgres.h>
#include <fmgr.h>

#ifdef PG_MODULE_MAGIC
    PG_MODULE_MAGIC;
#endif

typedef struct {
	char *tr_from;
	char *tr_to;
} translation;

static const translation translator[] = {
	{ "ä", "a" },
	{ "à", "a" },
	{ "á", "a" },
	{ "â", "a" },
	{ "ã", "a" },
	{ "å", "a" },
	{ "æ", "a" },

	{ "Ä", "A" },
	{ "À", "A" },
	{ "Á", "A" },
	{ "Â", "A" },
	{ "Ã", "A" },
	{ "Å", "A" },
	{ "Æ", "A" },

	{ "é", "e" },
	{ "è", "e" },
	{ "ê", "e" },
	{ "ë", "e" },

	{ "É", "E" },
	{ "È", "E" },
	{ "Ê", "E" },
	{ "Ë", "E" },

	{ "ì", "i" },
	{ "í", "i" },
	{ "î", "i" },
	{ "ï", "i" },

	{ "Ì", "I" },
	{ "Í", "I" },
	{ "Î", "I" },
	{ "Ï", "I" },
	
	{ "ö", "o" },
	{ "ò", "o" },
	{ "ó", "o" },
	{ "ô", "o" },
	{ "õ", "o" },
	{ "ø", "o" },
	{ "œ", "oe" },

	{ "Ö", "O" },
	{ "Ò", "O" },
	{ "Ó", "O" },
	{ "Ô", "O" },
	{ "Õ", "O" },
	{ "Ø", "O" },
	{ "Œ", "OE" },

	{ "ü", "u" },
	{ "ù", "u" },
	{ "ú", "u" },
	{ "û", "u" },

	{ "Ü", "U" },
	{ "Ù", "U" },
	{ "Ú", "U" },
	{ "Û", "U" },

	{ "ç", "c" },
	{ "ć", "c" },
	{ "č", "c" },

	{ "Ç", "C" },
	{ "Ć", "C" },
	{ "Č", "C" },

	{ "ð", "d" },

	{ "Ð", "D" },

	{ "ñ", "n" },

	{ "Ñ", "N" },

	{ "ý", "y" },
	{ "ÿ", "y" },

	{ "Ý", "Y" },
	{ "Ÿ", "Y" },

	{ "ś", "s" },
	{ "š", "s" },

	{ "Ś", "S" },
	{ "Š", "S" },

	{ "ź", "z" },
	{ "ž", "z" },

	{ "Ź", "Z" },
	{ "Ž", "Z" },

	{ "ß", "ss" },

	// Translate spaces to underscores
	{ " ", "_" },
	{ "\t", "_" },
	{ "\r", "_" },
	{ "\n", "_" },

	// Translate punctuation to underscores
	{ "/", "_" },
	{ ".", "_" },
	{ ":", "_" },
	{ ",", "_" },
	{ ";", "_" },
	{ "<", "_" },
	{ ">", "_" },
	{ "=", "_" },
	{ "\\", "_" },
	{ "#", "_" },
	{ "*", "_" },
	{ "\"", "_" },
	{ "'", "_" },
	{ "`", "_" },
	{ "(", "_" },
	{ ")", "_" },
	{ "[", "_" },
	{ "]", "_" },
	{ "{", "_" },
	{ "}", "_" },
	{ "|", "_" },
	{ "~", "_" },
	{ "^", "_" },

	// Translate choosen characters to urlencoded
	{ "!", "%21" },
	{ "$", "%24" },
	{ "%", "%25" },
	{ "&", "%26" },
	{ "+", "%2B" },
	{ "?", "%3F" },
	{ "@", "%40" },

	{ NULL, NULL }
};

Datum *tidyurl(PG_FUNCTION_ARGS)
{
	VarChar *arg;
	VarChar *ret;
	int ret_size;

	char *str;
	char *res;

	char *src, *dst;
	int in, out;
	translation *t;
	int size;

	arg = PG_GETARG_VARCHAR_P(0);

	// Extract string
	size = VARSIZE(arg) - VARHDRSZ;
	str = (char *) palloc(size + 1);
	if (str == NULL)
	{
		ereport(ERROR, (errcode(ERRCODE_OUT_OF_MEMORY),
			errmsg("out of memory")));
        	PG_RETURN_NULL();
	}

	memcpy(str, VARDATA(arg), size);
	str[size] = 0;

	// Allocate result string
	res = (char *) palloc(size + 1);
	if (res == NULL)
	{
		ereport(ERROR, (errcode(ERRCODE_OUT_OF_MEMORY),
			errmsg("out of memory")));
        	PG_RETURN_NULL();
	}

	memset(res, 0, size + 1);
	
	for (in = out = 0, src = str, dst = res; *src;
		src = str + in, dst = res + out)
	{
		// FIXME: Bubble search. fix for performance
		for (t = (translation *) translator; t->tr_from; ++t)
		{
			if (strncmp(src, t->tr_from, strlen(t->tr_from)) == 0)
			{
				src = t->tr_to;
				break;
			}
		}

		// Skip unknown/untranslated utf-8 chars and non printables
		if (*src <= 32 || *src >= 127)
		{
			++in;
			continue;
		}

		// Skip double white spaces: _
		if (out && *(dst - 1) == '_' && *src == '_')
		{
			++in;
			continue;
		}

		if (t->tr_from)
		{
			in += strlen(t->tr_from);
			out += strlen(t->tr_to);
		}
		else
		{
			++in;
			++out;
		}

		// Make sure theres enough room in result
		if (out >= size)
		{
			res = repalloc(res, size * 2 + 1);
			if (res == NULL)
			{
				ereport(ERROR, (errcode(ERRCODE_OUT_OF_MEMORY),
					errmsg("out of memory")));
				PG_RETURN_NULL();
			}

			memset(res + size, 0, size + 1);

			// Make sure dst points right
			dst = res + strlen(res);
			size *= 2;
		}

		if (t->tr_from)
		{
			strcpy(dst, src);
		}
		else
		{
			*dst = *src;
		}
	}

	// Remove trailing underscores (spaces)
	for (;*dst == '_'; --dst)
	{
		*dst = 0;
	}

	// Lowercase
	/*
	for (dst = res; *dst; ++dst)
	{
		if (isupper(*dst))
		{
			*dst = tolower(*dst);
		}
	}
	*/
	
	// Return result
	ret_size = out + VARHDRSZ;
	ret = (VarChar *) palloc(ret_size);
	if (ret == NULL)
	{
		ereport(ERROR, (errcode(ERRCODE_OUT_OF_MEMORY),
			errmsg("out of memory")));
		PG_RETURN_NULL();
	}

	SET_VARSIZE(ret, ret_size);
	memcpy(VARDATA(ret), res, ret_size);

	pfree(str);
	pfree(res);

	PG_RETURN_VARCHAR_P(ret);
}

PG_FUNCTION_INFO_V1(tidyurl);
