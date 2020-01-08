#include "Replace.h"

#define ERROR_SYMBOL L' '
#define PRECISION 0.99
#define DISPERSION 0.01

long double Rplce_GetExpectedValue(GlBrightness gb)
{
	long double ldResult =	(long double)gb.center + 
							(long double)gb.left + 
							(long double)gb.right + 
							(long double)gb.up + 
							(long double)gb.down;
	ldResult /= (long double)5.0;
	ldResult /= (long double)MAXBYTE;
	return ldResult;
}

SIZE_T Rplce_FindBestFitted(PBOOL pfIndexInclude, SIZE_T stArrLength, 
							long double ldExpectedValue, long double ldDispersion,
							PGlBrightness pgb)
{
	SIZE_T stCount = 0;
	for (SIZE_T i = 0; i < stArrLength; ++i)
	{
		stCount += pfIndexInclude[i];
	}
	
	if (stCount == 1)
	{
		for (SIZE_T i = 0; i < stArrLength; ++i)
		{
			if (pfIndexInclude[i])
			{
				return i;
			}
		}
	}
	else
	{
		PBYTE pbNumberOfFittedRegions = calloc(stArrLength, sizeof(BYTE));
		if (pbNumberOfFittedRegions == NULL)
		{
			return MAXSIZE_T;
		}

		BOOL fIsOneOrMoreMatch = FALSE;
		long double ldTmp = 0;
		do
		{
			ldTmp += ldDispersion;
			for (SIZE_T i = 0; i < stArrLength; ++i)
			{
				if (pfIndexInclude[i])
				{
					pbNumberOfFittedRegions[i] += ((pgb[i].up / (long double)MAXBYTE) > ldExpectedValue - ldTmp ||
													(pgb[i].up / (long double)MAXBYTE) < ldExpectedValue + ldTmp);

					pbNumberOfFittedRegions[i] += ((pgb[i].down / (long double)MAXBYTE) > ldExpectedValue - ldTmp ||
													(pgb[i].down / (long double)MAXBYTE) < ldExpectedValue + ldTmp);

					pbNumberOfFittedRegions[i] += ((pgb[i].left / (long double)MAXBYTE) > ldExpectedValue - ldTmp ||
													(pgb[i].left / (long double)MAXBYTE) < ldExpectedValue + ldTmp);

					pbNumberOfFittedRegions[i] += ((pgb[i].right / (long double)MAXBYTE) > ldExpectedValue - ldTmp ||
													(pgb[i].right / (long double)MAXBYTE) < ldExpectedValue + ldTmp);

					pbNumberOfFittedRegions[i] += ((pgb[i].center / (long double)MAXBYTE) > ldExpectedValue - ldTmp ||
													(pgb[i].center / (long double)MAXBYTE) < ldExpectedValue + ldTmp);

					fIsOneOrMoreMatch |= (pbNumberOfFittedRegions[i] > 0);
				}
			}
		} while (!fIsOneOrMoreMatch);

		SIZE_T stResult = 0;
		BYTE bMax = pbNumberOfFittedRegions[0];
		for (SIZE_T i = 1; i < stArrLength; ++i)
		{
			if (pbNumberOfFittedRegions[i] > bMax)
			{
				bMax = pbNumberOfFittedRegions[i];
				stResult = i;
			}
		}

		free(pbNumberOfFittedRegions);
		return stResult;
	}
}

SIZE_T Rplce_GetBestFittedSymbolIndex(CONST WCHAR szSymbols[], long double pldSymbolsExpectedValue[], 
									long double ldExpectedValue, long double ldDispersion,
									PGlBrightness pgb)
{
	SIZE_T stArrLength = wcslen(szSymbols);
	PBOOL pfIndexInclude = calloc(stArrLength, sizeof(BOOL));
	if (pfIndexInclude == NULL)
	{
		return MAXSIZE_T;
	}

	BOOL fIsOneOrMoreMatch = FALSE;
	long double ldTmp = 0;
	do
	{
		ldTmp += ldDispersion;
		for (SIZE_T i = 0; i < stArrLength; ++i)
		{
			pfIndexInclude[i] = (pldSymbolsExpectedValue[i] > ldExpectedValue - ldTmp &&
								 pldSymbolsExpectedValue[i] < ldExpectedValue + ldTmp);
			fIsOneOrMoreMatch |= pfIndexInclude[i];

		}
	} while (!fIsOneOrMoreMatch);
	
	SIZE_T stResult = Rplce_FindBestFitted(pfIndexInclude, stArrLength, ldExpectedValue, ldTmp, pgb);
	
	free(pfIndexInclude);
	return stResult;
}

WCHAR Replace_GetSymbol(CONST WCHAR szSymbols[], PGlBrightness pgb, GlBrightness gb)
{
	long double ldExpectedValue = Rplce_GetExpectedValue(gb);
	long double ldDispersion = DISPERSION /*ldExpectedValue * (PRECISION - ldExpectedValue * ldExpectedValue)*/;
	//ldDispersion = (ldDispersion < 0 ? -ldDispersion : ldDispersion);

	SIZE_T stArrLength = wcslen(szSymbols);
	long double *pldSymbolsExpectedValue = calloc(stArrLength, sizeof(long double));
	if (pldSymbolsExpectedValue == NULL)
	{
		return ERROR_SYMBOL;
	}

	for (SIZE_T i = 0; i < stArrLength; ++i)
	{
		pldSymbolsExpectedValue[i] = Rplce_GetExpectedValue(pgb[i]);
	}

	SIZE_T stIndex = Rplce_GetBestFittedSymbolIndex(szSymbols, pldSymbolsExpectedValue, ldExpectedValue, ldDispersion, pgb);
	WCHAR wcResult = ERROR_SYMBOL;
	if (stIndex != MAXSIZE_T)
	{
		wcResult = szSymbols[stIndex];
	}

	free(pldSymbolsExpectedValue);
	return wcResult;
}


