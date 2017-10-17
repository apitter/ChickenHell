#include <game_scores.h>

// time bonuses - in msec
#define MAX_TIME_LEVEL1 (30)  // 30 sec
#define MAX_TIME_LEVEL2 (45)  // 45 sec
#define MAX_TIME_LEVEL3 (60)  // 60 sec
#define MAX_TIME_LEVEL4 (75)  // 90 sec
#define MAX_TIME_LEVEL5 (90) // 120 sec
// point per egg
#define POINT_PER_EGG   (5)

namespace Game
{
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
Statistic::Statistic()
{
    clear();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void Statistic::clear()
{
    m_distance = 0;
    m_eggs = 0;
    m_gameLevel = (int)Game::Level::First;
    m_mazeLevel = (int)Game::Level::First;
    m_tot_distance = 0;
    m_scores = 0;
    m_tick_count = 0;
    m_distanceBonus = 0;
    m_timeBonus = 0;
    m_mazeDistance = 0;
    Scene *scene = Scene::getInstance();
    m_time = scene->getRendererTime();
    m_tick_start = scene->getTickCount();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void Statistic::updateDistance(ngl::Real _deltaDistance)
{
    m_distance += _deltaDistance;
    m_tot_distance += _deltaDistance;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void Statistic::levelUp()
{
    static int max_ticks[5] = {
        MAX_TIME_LEVEL1 * 1000
        , MAX_TIME_LEVEL2 * 1000
        , MAX_TIME_LEVEL3 * 1000
        , MAX_TIME_LEVEL4 * 1000
        , MAX_TIME_LEVEL5 * 1000
    };
    // increment the level
    if(m_gameLevel < (int)Game::Level::Last)
    {
        m_mazeLevel++;
    }
    m_gameLevel++;
    // setup the time counters
    int level_ticks = m_time.elapsed() - m_tick_start;
    m_tick_count += level_ticks;
    m_tick_start = m_time.elapsed();
    // compute the bonuses of the level
    m_distanceBonus = m_mazeDistance + (m_mazeDistance*(m_mazeLevel-1)/10) - m_distance;
    if(m_distanceBonus < 0) { m_distanceBonus = 0; }
    m_timeBonus = (max_ticks[m_mazeLevel-2] - level_ticks) / 1000;
    if(m_timeBonus < 0) { m_timeBonus = 0; }
    m_distance = 0;
    // add the bonus
    m_scores += m_distanceBonus + m_timeBonus;
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//
std::string Statistic::getGameTime()
{
    std::string s;
    if(m_time.isValid())
    {
        int cur_tick = m_tick_count + m_time.elapsed() - m_tick_start;
        QTime t((cur_tick / (1000 * 60 * 60)), (cur_tick / 60000) % 60, (cur_tick / 1000) % 60, cur_tick % 1000);
        s = (t.isValid()) ? t.toString("hh:mm:ss").toStdString() : "00:00:00";
    }
    else
    {
        s = "00:00:00";
    }
    return s;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Statistic::addEgg()
{
    m_eggs += 1;
    m_scores += POINT_PER_EGG;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
} // end Game
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
