// FPConv.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#include <direct.h>
#include <list>
#include <Windows.h>

#define MAKE_WP_TYPE(ch1, ch2)		(((ch1) << 8) | (ch2))
#define NRESULT_OK				0
#define countof(x)			(sizeof(x) / sizeof(x[0]))

typedef int NRESULT;

struct SFPLWaypoint
{
	BYTE	byType;
	char	szName[6];
	float	fLat, fLon;
	INT		nHeight;
};

typedef std::list<SFPLWaypoint>		FLIGHTPLAN;

static FLIGHTPLAN	s_fpl;

void printUsage()
{
	printf("USAGE: FPConv.exe <infile> <cycle> [<out path>]\n\n");
	printf("<infile>   - Jeppesen FliteStar/FliteMap flight plan in generic text format.\n");
	printf("<cycle>    - 4-symbol cycle number, i.e. '1711', '1802' and so on\n");
	printf("<out path> - Path to output .fms files.\n");
}

NRESULT readPlan(FILE* hFile, FLIGHTPLAN* pFpl)
{
	SFPLWaypoint	wp;
	NRESULT	nRes = NRESULT_OK;
	INT		n;
	float	f;
	char	sz[128], *psz;
	
	pFpl->clear();

	while (fgets(sz, countof(sz), hFile))
	{
		// Если встретился FPL - выходим с мыслью вернуться и продолжать
		if (!strncmp(sz, "FPL", 3))
		{
			nRes = 1;
			break;
		}
		else if (!strncmp(sz, "FWP", 3))
		{
			n = MAKE_WP_TYPE(sz[4], sz[6]);

			switch (n)
			{
			case MAKE_WP_TYPE('p', 'a'):	// Airport
				wp.byType = 1;
				break;
			case MAKE_WP_TYPE('p', 'n'):	// NDB
				wp.byType = 2;
				break;
			case MAKE_WP_TYPE('p', 'v'):	// VOR
				wp.byType = 3;
				break;
			case MAKE_WP_TYPE('p', 'i'):	// Intersect
				wp.byType = 11;
				break;
			case MAKE_WP_TYPE('u', 's'):	// Airport
				wp.byType = 28;
				break;
			default:
				printf("Wrong waypoint type: %s", sz);
				nRes = -1;
			}

			if (nRes) break;

			strncpy(wp.szName, &sz[8], countof(wp.szName));
			psz = strchr(wp.szName, ' ');
			if (psz) *psz = 0;

			sscanf(&sz[16], "%d %f", &n, &f);
			f = n + f / 60;

			if (sz[14] == 'S') f *= -1;
			else if (sz[14] != 'N')
			{
				printf("Wrong symbol in 14-th column (latitude): %s", sz);
				nRes = -2;
				break;
			}

			wp.fLat = f;

			sscanf(&sz[27], "%d %f", &n, &f);
			f = n + f / 60;

			if (sz[25] == 'W') f *= -1;
			else if (sz[25] != 'E')
			{
				printf("Wrong symbol in 25-th column (longitude): %s", sz);
				nRes = -3;
				break;
			}

			wp.fLon = f;

			sscanf(&sz[37], "%d", &wp.nHeight);

			pFpl->push_back(wp);
		}
	}

	return nRes;
}

NRESULT writePlan(FLIGHTPLAN* pFpl, char* szCycle)
{
	char	sz[32];
	FLIGHTPLAN::iterator wp, wplast;
	//SFPLWaypoint		 wp;
	FILE*	hFile;
	INT		n;
	
	if (pFpl->empty()) return NRESULT_OK;

	sprintf(sz, "%s-%s.fms", pFpl->front().szName, pFpl->back().szName);

	hFile = fopen(sz, "w+");
	if (!hFile)
	{
		printf("Output file open error\n");
		return -5;
	}

	fprintf(hFile, "I\n1100 Version\nCYCLE %s\n", szCycle);
	
	// Departure
	wp = pFpl->begin();
	fprintf(hFile, "%sDEP %s\n", wp->byType == 1 ? "A" : "", wp->szName);
	// Destination
	wp = pFpl->end();	wplast = --wp;
	fprintf(hFile, "%sDES %s\n", wp->byType == 1 ? "A" : "", wp->szName);
	// Waypoints
	n = pFpl->size();
	if (n > 1)
	{
		fprintf(hFile, "NUMENR %d\n", n);

		for (wp = pFpl->begin(); wp != pFpl->end(); ++wp)
		{
			fprintf(hFile, "%d %s", wp->byType, wp->szName);

			if (wp == pFpl->begin()) fprintf(hFile, " ADEP");
			else if (wp == wplast)	 fprintf(hFile, " ADES");
			else					 fprintf(hFile, " DRCT");

			fprintf(hFile, " %.6f %.6f %.6f\n", (float)wp->nHeight, wp->fLat, wp->fLon);
		}
	}

	fclose(hFile);

	printf("%s written\n", sz);

	return NRESULT_OK;
}

int _tmain(int argc, _TCHAR* argv[])
{
	char	szCD[256];
	FILE*	hFile;
	NRESULT nRes;
	INT		n;

	printf("Jeppesen FliteStar/FliteMap to X-plane FMS flight plan converter\n\n");

	if (argc < 3 || argc > 4)
	{
		printUsage();
		return -1;
	}

	if (!_getcwd(szCD, countof(szCD))) return -5;

	hFile = fopen(argv[1], "r");
	if (!hFile)
	{
		printf("ERROR: couldn't open file \"%s\"\n\n", argv[1]);
		return -2;
	}

	do
	{
		nRes = readPlan(hFile, &s_fpl);
		
		if (argc == 4)
		{
			n = _chdir(argv[3]);
			if (n < 0)
			{
				printf("Output dir open error\n");
				return -4;
			}
		}

		if (writePlan(&s_fpl, argv[2])) break;
	
		if (argc == 4) _chdir(szCD);

	} while (nRes > 0);

	fclose(hFile);
    return 0;
}

