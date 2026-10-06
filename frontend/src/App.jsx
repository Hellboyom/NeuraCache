import {
  Activity,
  Brain,
  Database,
  Gauge,
  Server,
  Zap,
  ArrowUpRight,
  Circle
} from "lucide-react";
          
import {
  LineChart,
  Line,
  XAxis,
  YAxis,
  CartesianGrid,
  Tooltip,
  ResponsiveContainer
} from "recharts";

import { useEffect, useState } from "react";

import "./App.css";


const API_URL =
  "http://localhost:3001/api/dashboard";


function StatCard({
  icon: Icon,
  label,
  value,
  detail
}) {
  return (
    <div className="stat-card">

      <div className="stat-top">

        <div className="stat-icon">
          <Icon size={20} />
        </div>

        <span className="stat-detail">
          {detail}
        </span>

      </div>

      <div className="stat-value">
        {value}
      </div>

      <div className="stat-label">
        {label}
      </div>

    </div>
  );
}


function PredictionRow({
  name,
  score,
  accesses
}) {
  const percentage =
    Math.min(Math.max(score * 100, 0), 100);

  return (
    <div className="prediction-row">

      <div className="prediction-main">

        <div className="prediction-top">

          <div>
            <div className="prediction-name">
              {name}
            </div>

            <div className="prediction-access">
              {accesses} accesses
            </div>
          </div>

          <div className="prediction-score">
            {score.toFixed(3)}
          </div>

        </div>

        <div className="prediction-bar">
          <div
            className="prediction-bar-fill"
            style={{
              width: `${percentage}%`
            }}
          />
        </div>

      </div>

    </div>
  );
}


function App() {

  const [data, setData] =
    useState(null);
  const [activityHistory, setActivityHistory] = 
    useState([]);

  const [online, setOnline] =
    useState(false);

  const [error, setError] =
    useState(null);


  async function fetchDashboard() {

    try {

      const response =
        await fetch(API_URL);

      if (!response.ok) {
        throw new Error(
          "API request failed"
        );
      }

      const result =
        await response.json();

      setData(result);

      setOnline(true);

setActivityHistory((previous) => {
  const next = [
    ...previous,
    {
      time: new Date().toLocaleTimeString([], {
        minute: "2-digit",
        second: "2-digit"
      }),
      commands: Number(result.info?.total_commands || 0),
      hits: Number(result.info?.cache_hits || 0),
      misses: Number(result.info?.cache_misses || 0)
    }
  ];

  return next.slice(-30);
});

setError(null);

      setError(null);

    } catch (err) {

      console.error(err);

      setOnline(false);

      setError(
        "Unable to connect to NeuraCache"
      );
    }
  }


  useEffect(() => {

    fetchDashboard();

    const interval =
      setInterval(
        fetchDashboard,
        2000
      );

    return () =>
      clearInterval(interval);

  }, []);


  if (!data) {

    return (
      <div className="loading">
        Connecting to NeuraCache...
      </div>
    );
  }


  const info =
    data.info || {};

  const analyze =
    data.analyze || {};


  const hits =
    Number(
      info.cache_hits || 0
    );

  const misses =
    Number(
      info.cache_misses || 0
    );

  const totalRequests =
    hits + misses;


  const hitRate =
    totalRequests > 0
      ? (
          hits /
          totalRequests *
          100
        ).toFixed(1)
      : "0.0";


  const totalCommands =
    Number(
      info.total_commands || 0
    );


  const evictions =
    Number(
      info.evictions || 0
    );


  const uptime =
    Number(
      info.uptime_seconds || 0
    );


  const predictions =
    analyze.predictions || [];


  return (

    <div className="app">


      {/* Sidebar */}

      <aside className="sidebar">

        <div className="brand">

          <div className="brand-mark">
            N
          </div>

          <div>

            <div className="brand-name">
              NeuraCache
            </div>

            <div className="brand-subtitle">
              Adaptive Memory Engine
            </div>

          </div>

        </div>


        <nav className="navigation">

          <div className="nav-item active">
            <Gauge size={18} />
            Dashboard
          </div>

          <div className="nav-item">
            <Database size={18} />
            Cache
          </div>

          <div className="nav-item">
            <Brain size={18} />
            AI Predictor
          </div>

          <div className="nav-item">
            <Activity size={18} />
            Metrics
          </div>

        </nav>


        <div className="sidebar-bottom">

          <div className="server-status">

            <Circle
              size={9}
              fill="currentColor"
            />

            <span>
              {online
                ? "System Online"
                : "System Offline"}
            </span>

          </div>

          <div className="version">
            NeuraCache v1.0
          </div>

        </div>

      </aside>


      {/* Main */}

      <main className="main">


        <header className="header">

          <div>

            <div className="eyebrow">
              SYSTEM OVERVIEW
            </div>

            <h1>
              Cache Dashboard
            </h1>

            <p>
              Live overview of your adaptive
              in-memory database.
            </p>

          </div>


          <div className="header-status">

            <span className="online-dot" />

            {online
              ? "RUNNING"
              : "OFFLINE"}

          </div>

        </header>


        {/* Statistics */}

        <section className="stats-grid">


          <StatCard
            icon={Zap}
            label="Cache Hit Rate"
            value={`${hitRate}%`}
            detail={
              totalRequests
                ? `${hits} hits`
                : "No requests"
            }
          />


          <StatCard
            icon={Activity}
            label="Total Commands"
            value={
              totalCommands.toLocaleString()
            }
            detail={
              `${uptime}s uptime`
            }
          />


          <StatCard
            icon={ArrowUpRight}
            label="Evictions"
            value={
              evictions.toLocaleString()
            }
            detail="Adaptive"
          />


          <StatCard
            icon={Server}
            label="Tracked AI Keys"
            value={
              Number(
                analyze.trackedKeys || 0
              )
            }
            detail="Predictor"
          />


        </section>


        {/* Main panels */}

        <section className="content-grid">


          <div className="panel activity-panel">

            <div className="panel-header">

              <div>

                <h2>
                  Cache Activity
                </h2>

                <span>
                  Live system state
                </span>

              </div>

              <div className="live-indicator">

                <span />

                LIVE

              </div>

            </div>


            <div className="activity-content">

              <div className="activity-stat">

                <span>
                  GET requests
                </span>

                <strong>
                  {info.get_commands || 0}
                </strong>

              </div>


              <div className="activity-stat">

                <span>
                  SET requests
                </span>

                <strong>
                  {info.set_commands || 0}
                </strong>

              </div>


              <div className="activity-stat">

                <span>
                  Cache hits
                </span>

                <strong>
                  {hits}
                </strong>

              </div>


              <div className="activity-stat">

                <span>
                  Cache misses
                </span>

                <strong>
                  {misses}
                </strong>

              </div>


              <div className="activity-stat">

                <span>
                  Expired keys
                </span>

                <strong>
                  {info.expired_keys || 0}
                </strong>

              </div>

              <div className="activity-stat">
                <span>Evictions</span>
                <strong>{data.info.evictions || 0}</strong>
              </div>


            </div>

          </div>


          {/* AI Predictions */}

          <div className="panel">

            <div className="panel-header">

              <div>

                <h2>
                  AI Predictions
                </h2>

                <span>
                  Highest predicted access
                </span>

              </div>

              <Brain size={20} />

            </div>


            <div className="prediction-list">

              {predictions.length === 0 ? (

                <div className="empty-state">
                  No prediction data yet.
                  <br />
                  Generate some cache activity.
                </div>

              ) : (

                predictions.map(
                  (prediction) => (

                    <PredictionRow
                      key={prediction.key}
                      name={prediction.key}
                      score={prediction.score}
                      accesses={
                        prediction.accesses
                      }
                    />

                  )
                )

              )}

            </div>

          </div>

        </section>


        {/* Bottom section */}

        <section className="bottom-grid">


          <div className="panel">

            <div className="panel-header">

              <div>

                <h2>
                  Cache Metrics
                </h2>

                <span>
                  Current runtime statistics
                </span>

              </div>

            </div>


            <div className="metric-list">

  <div>
    <span>
      Total commands
    </span>

    <strong>
      {info.total_commands || 0}
    </strong>
  </div>

  <div>
    <span>
      GET commands
    </span>

    <strong>
      {info.get_commands || 0}
    </strong>
  </div>

  <div>
    <span>
      SET commands
    </span>

    <strong>
      {info.set_commands || 0}
    </strong>
  </div>

  <div>
    <span>
      DELETE commands
    </span>

    <strong>
      {info.del_commands || 0}
    </strong>
  </div>

  <div>
    <span>
      Cache hit rate
    </span>

    <strong>
      {hitRate}%
    </strong>
  </div>

  <div>
    <span>
      Cache evictions
    </span>

    <strong>
      {info.evictions || 0}
    </strong>
  </div>

  <div>
    <span>
      Expired keys
    </span>

    <strong>
      {info.expired_keys || 0}
    </strong>
  </div>

  <div>
    <span>
      AI tracked keys
    </span>

    <strong>
      {analyze.trackedKeys || 0}
    </strong>
  </div>

  <div>
    <span>
      AI accesses
    </span>

    <strong>
      {analyze.totalAccesses || 0}
    </strong>
  </div>

  <div>
    <span>
      Uptime
    </span>

    <strong>
      {uptime}s
    </strong>
  </div>

  </div>
</div>


          <div className="panel ai-panel">

            <div className="ai-header">

              <div className="ai-icon">
                <Brain size={22} />
              </div>

              <div>

                <h2>
                  AI Cache Analysis
                </h2>

                <span>
                  Adaptive predictor
                </span>

              </div>

            </div>


            {predictions.length > 0 ? (
  <div className="ai-analysis-content">

    <div className="ai-highlight">
      <div>
        <span className="ai-label">
          TOP PREDICTION
        </span>

        <strong className="ai-key">
          {predictions[0].key}
        </strong>

        <span className="ai-access-count">
          {predictions[0].accesses} observed accesses
        </span>
      </div>

      <div className="ai-score">
        {predictions[0].score.toFixed(3)}
        <span>score</span>
      </div>
    </div>

    <div className="ai-score-bar">
      <div
        className="ai-score-fill"
        style={{
          width: `${Math.min(
            predictions[0].score * 100,
            100
          )}%`
        }}
      />
    </div>

    <p className="ai-explanation">
      The adaptive predictor currently identifies{" "}
      <strong>
        {predictions[0].key}
      </strong>{" "}
      as the highest-priority key based on
      observed access frequency and recent activity.
    </p>

  </div>
) : (
  <p>
    The AI predictor is waiting for access
    activity before generating predictions.
  </p>
)}


            <div className="ai-footer">

              <span>
                Total predicted accesses
              </span>

              <strong>
                {analyze.totalAccesses || 0}
              </strong>

            </div>

          </div>


        </section>
        <section className="panel activity-chart-panel">
  <div className="panel-header">
    <div>
      <h2>Command Activity</h2>
      <p>Live request volume</p>
    </div>

    <span className="live-indicator">
      <span className="live-dot"></span>
      LIVE
    </span>
  </div>

  <div className="activity-chart">
    {activityHistory.length > 1 ? (
      <ResponsiveContainer width="100%" height={280}>
        <LineChart data={activityHistory}>
          <CartesianGrid
            strokeDasharray="3 3"
            stroke="#1b1f29"
          />

          <XAxis
            dataKey="time"
            stroke="#59616e"
            tick={{ fontSize: 10 }}
            tickLine={false}
            axisLine={false}
          />

          <YAxis
            stroke="#59616e"
            tick={{ fontSize: 10 }}
            tickLine={false}
            axisLine={false}
            allowDecimals={false}
          />

          <Tooltip
            contentStyle={{
              background: "#0d1016",
              border: "1px solid #242936",
              borderRadius: "8px",
              color: "#e8eaf0",
              fontSize: "11px"
            }}
          />

          <Line
            type="monotone"
            dataKey="commands"
            name="Commands"
            stroke="#8b7cff"
            strokeWidth={2}
            dot={false}
            activeDot={{ r: 4 }}
          />

          <Line
            type="monotone"
            dataKey="hits"
            name="Cache Hits"
            stroke="#54e6a8"
            strokeWidth={2}
            dot={false}
            activeDot={{ r: 4 }}
          />

          <Line
            type="monotone"
            dataKey="misses"
            name="Cache Misses"
            stroke="#ff7181"
            strokeWidth={2}
            dot={false}
            activeDot={{ r: 4 }}
          />
        </LineChart>
      </ResponsiveContainer>
    ) : (
      <div className="chart-empty">
        Collecting live activity data...
      </div>
    )}
  </div>
</section>  


        {error && (

          <div className="error-banner">
            {error}
          </div>

        )}

      </main>

    </div>
  );
}


export default App;