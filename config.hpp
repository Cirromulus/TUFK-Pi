#pragma once
static constexpr unsigned EXPANDER_ADDR = 0x27;
static constexpr unsigned EXPANDER_BASE_PIN = 100;
static constexpr unsigned EXPANDER_ITB  = 7;

static constexpr unsigned HEATERPIN     = EXPANDER_BASE_PIN + 0;
static constexpr unsigned VENTILATORPIN = EXPANDER_BASE_PIN + 1;
static constexpr unsigned FRIDGEPIN     = EXPANDER_BASE_PIN + 2;
static constexpr unsigned PAAMPPIN      = EXPANDER_BASE_PIN + 3;

static constexpr unsigned GREENLED      = EXPANDER_BASE_PIN + 8 + 0;
static constexpr unsigned WHITELED      = EXPANDER_BASE_PIN + 8 + 1;
static constexpr unsigned REDLED        = EXPANDER_BASE_PIN + 8 + 2;
static constexpr unsigned ADDNLLED1     = EXPANDER_BASE_PIN + 8 + 3;
static constexpr unsigned ADDNLLED2     = EXPANDER_BASE_PIN + 8 + 4;
static constexpr unsigned ADDNLLED3     = EXPANDER_BASE_PIN + 8 + 5;
static constexpr unsigned ALARMPIN      = EXPANDER_BASE_PIN + 8 + 6;

static constexpr unsigned PIRPIN    = 0;
static constexpr unsigned IRLED     = 1;
static constexpr unsigned GASSENS   = 2;
static constexpr unsigned IRSENSPIN = 3;
static constexpr unsigned DHT22PIN  = 4;