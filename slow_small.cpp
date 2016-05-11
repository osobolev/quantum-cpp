#include "small_entry.h"

typedef struct {
  double amp;
  bool present;
} Element;

typedef Element Elements[2];

static Elements *matrix;
static Elements *amplitudes;
static int *dirs;
#ifdef CHECK_UNDERFLOW
static double *sumAbs;
#endif

static double ampTol;

void initEntries(Index maxEntries, double ampTol)
{
  ::ampTol = ampTol;
  printf("Entry size = %d\n", (int) sizeof(Entry));
  initList(maxEntries, sizeof(Entry));

  amplitudes = (Elements *) malloc(ofs(edgeNum, sizeof(Elements)));
  dirs = (int *) malloc(ofs(edgeNum, sizeof(int)));
#ifdef CHECK_UNDERFLOW
  sumAbs = (double *) malloc(ofs(edgeNum, sizeof(double)));
#endif

  matrix = (Elements *) malloc(ofs(vertexNum * edgeNum, sizeof(Elements)));

  initInternal();
}

template <class T> inline T* getRow(T *arr, int i)
{
  assert(i >= 0 && i < vertexNum);
  return &arr[i * edgeNum];
}

static void deleteFirst(Index n)
{
  while (n > 0) {
    deleteFirst();
    n--;
  }
}

void schedule(Number time, Direction direction, double amp, int edge)
{
  binarySearch(time, &found);
  Number timeToInsert;
  if (!found.exists) {
    // all entries with i < insert have time less than 'time'
    // all entries with i >= insert have time greater than 'time'
    bool addTo = false;
    Number entryTime;
    Entry *entry = (Entry *) getAt(found, &entryTime);
    if (entry) {
      if (isNear(time, entryTime)) {
        addTo = true;
        timeToInsert = entryTime;
      }
    }
    if (!addTo) {
      Number prevEntryTime;
      Entry *prevEntry = (Entry *) getBefore(found, &prevEntryTime);
      if (prevEntry) {
        if (isNear(time, prevEntryTime)) {
          addTo = true;
          timeToInsert = prevEntryTime;
        }
      }
    }
    if (!addTo) {
      timeToInsert = time;
    }
  } else {
    timeToInsert = time;
  }
  Entry *newEntry = (Entry *) insertIntoList(timeToInsert);
  if (newEntry == NULL) {
    printf("Out of memory!!!\n");
    exit(1);
  }
  newEntry->amp = amp;
  newEntry->direction = direction;
  newEntry->edge = edge;
}

void next()
{
  Number entryTime = getFirst();
  currentTime = entryTime;
  memset(matrix, 0, vertexNum * edgeNum * sizeof(Elements));
  Iterator iter;
  initIter(&iter);
  int count = 0;
  while (true) {
    Number iTime;
    Entry *ie = (Entry *) getAt(iter, &iTime);
    if (iTime != entryTime)
      break;
    int vertex = edgeSide(ie->edge, ie->direction);
    Element *elem = &getRow(matrix, vertex)[ie->edge][ie->direction];
    if (elem->present) {
      double a1 = elem->amp;
      double a2 = ie->amp;
      elem->amp = sqrt(a1 * a1 + a2 * a2);
    } else {
      elem->present = true;
      elem->amp = ie->amp;
    }
    count++;
    if (!moveForward(&iter))
      break;
  }
  deleteFirst(count);
  for (int vertex = 0; vertex < vertexNum; vertex++) {
    int e;
    int outLength;
    int *out = goingOut(vertex, &outLength);
    assert(outLength <= degree);
    memset(amplitudes, 0, ofs(edgeNum, sizeof(Elements)));
#ifdef CHECK_UNDERFLOW
    memset(sumAbs, 0, ofs(edgeNum, sizeof(double)));
#endif
    if (outLength == 1) {
      int edge = out[0];
      Direction direction = destination(vertex, edge);
      Element *elem = &getRow(matrix, vertex)[edge][reverse(direction)];
      if (elem->present) {
        amplitudes[edge][direction].present = true;
        amplitudes[edge][direction].amp -= elem->amp;
#ifdef CHECK_UNDERFLOW
        sumAbs[edge] += fabs(elem->amp);
#endif
      }
    } else {
      Elements *rows = getRow(matrix, vertex);
      double k = 2.0 / outLength;
      for (e = 0; e < edgeNum; e++) {
        Element *row = rows[e];
        for (int dir = 0; dir < 2; dir++) {
          if (!row[dir].present)
            continue;
          double amp = row[dir].amp;
          double forward = k * amp;
          memset(dirs, 0, ofs(edgeNum, sizeof(int)));
          for (int j = 0; j < outLength; j++) {
            int edge = out[j];
            double add;
            int outDir;
            if (isLoop(edge)) {
              outDir = flip(dirs[edge]);
            } else {
              outDir = destination(vertex, edge);
            }
            if (e == edge && dir != outDir) {
              add = (k - 1) * amp;
            } else {
              add = forward;
            }
            amplitudes[edge][outDir].present = true;
            amplitudes[edge][outDir].amp += add;
#ifdef CHECK_UNDERFLOW
            sumAbs[edge] += fabs(add);
#endif
          }
        }
      }
    }
    for (e = 0; e < edgeNum; e++) {
      for (int dir = 0; dir < 2; dir++) {
        if (!amplitudes[e][dir].present)
          continue;
        double amp = amplitudes[e][dir].amp;
#ifdef CHECK_UNDERFLOW
        if (fabs(amp) == 0) {
          if (sumAbs[e] > 0) {
            printf("%lg\n", sumAbs[e]);
            continue;
          } else {
            printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
          }
        }
#endif
        if (fabs(amp) <= ampTol)
          continue;
        Number len = getEdgeLength(e);
        Number nextTime = currentTime + len;
        (*getRadiation(vertex, e))++;
        schedule(nextTime, (Direction) dir, amp, e);
      }
    }
  }
}

static void statFunc(void *e)
{
  Entry *entry = (Entry *) e;
  int edge = entry->edge;
  double amp = entry->amp;
  double e1 = amp * amp;
  photonCount[edge]++;
  photonEnergy[edge] += e1;
  totalPhotonCount++;
  totalPhotonEnergy += e1;
}
