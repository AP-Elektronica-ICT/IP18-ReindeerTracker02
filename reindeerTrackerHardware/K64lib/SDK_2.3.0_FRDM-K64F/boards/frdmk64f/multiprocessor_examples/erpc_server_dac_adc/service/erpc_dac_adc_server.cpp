/*
 * The Clear BSD License
 * Copyright (c) 2014-2016, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 * that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Generated by erpcgen 1.5.0 on Mon Nov 13 15:29:32 2017.
 *
 * AUTOGENERATED - DO NOT EDIT
 */


#include "erpc_dac_adc_server.h"
#include <new>
#include "erpc_port.h"

#if 10500 != ERPC_VERSION_NUMBER
#error "The generated shim code version is different to the rest of eRPC code."
#endif


#if !defined(ERPC_GENERATED_CRC) || ERPC_GENERATED_CRC != 40084
#error "Macro 'ERPC_GENERATED_CRC' should be defined with value 40084."
#endif


using namespace erpc;
using namespace std;

#if ERPC_NESTED_CALLS_DETECTION
extern bool nestingDetection;
#endif

// for mdk/keil do not forgett add "--muldefweak" for linker
extern const uint32_t erpc_generated_crc;
#pragma weak erpc_generated_crc
extern const uint32_t erpc_generated_crc = 40084;


// Constant variable definitions
#pragma weak StringMaxSize
extern const uint8_t StringMaxSize = 11;

//! @brief Function to write struct AdcConfig
static int32_t write_AdcConfig_struct(erpc::Codec * codec, const AdcConfig * data);

//! @brief Function to write struct Vector
static int32_t write_Vector_struct(erpc::Codec * codec, const Vector * data);


// Write struct AdcConfig function implementation
static int32_t write_AdcConfig_struct(erpc::Codec * codec, const AdcConfig * data)
{
    erpc_status_t err = codec->startWriteStruct();
    if (!err)
    {
        err = codec->write(data->vref);
    }

    if (!err)
    {
        err = codec->write(data->atomicSteps);
    }

    if (!err)
    {
        err = codec->endWriteStruct();
    }
    return err;
}

// Write struct Vector function implementation
static int32_t write_Vector_struct(erpc::Codec * codec, const Vector * data)
{
    erpc_status_t err = codec->startWriteStruct();
    if (!err)
    {
        err = codec->write(data->A_x);
    }

    if (!err)
    {
        err = codec->write(data->A_y);
    }

    if (!err)
    {
        err = codec->write(data->A_z);
    }

    if (!err)
    {
        err = codec->write(data->M_x);
    }

    if (!err)
    {
        err = codec->write(data->M_y);
    }

    if (!err)
    {
        err = codec->write(data->M_z);
    }

    if (!err)
    {
        err = codec->endWriteStruct();
    }
    return err;
}


// Call the correct server shim based on method unique ID.
erpc_status_t dac_adc_service::handleInvocation(uint32_t methodId, uint32_t sequence, Codec * codec, MessageBufferFactory *messageFactory)
{
    switch (methodId)
    {
        case kdac_adc_adc_get_config_id:
            return adc_get_config_shim(codec, messageFactory, sequence);

        case kdac_adc_convert_dac_adc_id:
            return convert_dac_adc_shim(codec, messageFactory, sequence);

        case kdac_adc_set_led_id:
            return set_led_shim(codec, messageFactory, sequence);

        case kdac_adc_read_senzor_mag_accel_id:
            return read_senzor_mag_accel_shim(codec, messageFactory, sequence);

        case kdac_adc_board_get_name_id:
            return board_get_name_shim(codec, messageFactory, sequence);

        default:
            return kErpcStatus_InvalidArgument;
    }
}

// Server shim for adc_get_config of dac_adc interface.
erpc_status_t dac_adc_service::adc_get_config_shim(Codec * codec, MessageBufferFactory *messageFactory, uint32_t sequence)
{
    erpc_status_t err = kErpcStatus_Success;

    AdcConfig *config = NULL;

    // startReadMessage() was already called before this shim was invoked.

    if (!err)
    {
        err = codec->endReadMessage();
    }

    config = (AdcConfig *) erpc_malloc(sizeof(AdcConfig));
    if (config == NULL)
    {
        err = kErpcStatus_MemoryError;
    }

    // Invoke the actual served function.
#if ERPC_NESTED_CALLS_DETECTION
    nestingDetection = true;
#endif
    if (!err)
    {
        adc_get_config(config);
    }
#if ERPC_NESTED_CALLS_DETECTION
    nestingDetection = false;
#endif

    // preparing MessageBuffer for serializing data
    if (!err)
    {
        err = messageFactory->prepareServerBufferForSend(codec->getBuffer());
    }

    // preparing codec for serializing data
    codec->reset();

    // Build response message.
    if (!err)
    {
        err = codec->startWriteMessage(kReplyMessage, kdac_adc_service_id, kdac_adc_adc_get_config_id, sequence);
    }

    if (!err)
    {
        err = write_AdcConfig_struct(codec, config);
    }

    if (!err)
    {
        err = codec->endWriteMessage();
    }

    if (config)
    {
        erpc_free(config);
    }
    return err;
}

// Server shim for convert_dac_adc of dac_adc interface.
erpc_status_t dac_adc_service::convert_dac_adc_shim(Codec * codec, MessageBufferFactory *messageFactory, uint32_t sequence)
{
    erpc_status_t err = kErpcStatus_Success;

    uint32_t numberToConvert;
    uint32_t result;

    // startReadMessage() was already called before this shim was invoked.

    if (!err)
    {
        err = codec->read(&numberToConvert);
    }

    if (!err)
    {
        err = codec->endReadMessage();
    }

    // Invoke the actual served function.
#if ERPC_NESTED_CALLS_DETECTION
    nestingDetection = true;
#endif
    if (!err)
    {
        convert_dac_adc(numberToConvert, &result);
    }
#if ERPC_NESTED_CALLS_DETECTION
    nestingDetection = false;
#endif

    // preparing MessageBuffer for serializing data
    if (!err)
    {
        err = messageFactory->prepareServerBufferForSend(codec->getBuffer());
    }

    // preparing codec for serializing data
    codec->reset();

    // Build response message.
    if (!err)
    {
        err = codec->startWriteMessage(kReplyMessage, kdac_adc_service_id, kdac_adc_convert_dac_adc_id, sequence);
    }

    if (!err)
    {
        err = codec->write(result);
    }

    if (!err)
    {
        err = codec->endWriteMessage();
    }

    return err;
}

// Server shim for set_led of dac_adc interface.
erpc_status_t dac_adc_service::set_led_shim(Codec * codec, MessageBufferFactory *messageFactory, uint32_t sequence)
{
    erpc_status_t err = kErpcStatus_Success;

    uint8_t whichLed;

    // startReadMessage() was already called before this shim was invoked.

    if (!err)
    {
        err = codec->read(&whichLed);
    }

    if (!err)
    {
        err = codec->endReadMessage();
    }

    // Invoke the actual served function.
#if ERPC_NESTED_CALLS_DETECTION
    nestingDetection = true;
#endif
    if (!err)
    {
        set_led(whichLed);
    }
#if ERPC_NESTED_CALLS_DETECTION
    nestingDetection = false;
#endif

    // preparing MessageBuffer for serializing data
    if (!err)
    {
        err = messageFactory->prepareServerBufferForSend(codec->getBuffer());
    }

    // preparing codec for serializing data
    codec->reset();

    // Build response message.
    if (!err)
    {
        err = codec->startWriteMessage(kReplyMessage, kdac_adc_service_id, kdac_adc_set_led_id, sequence);
    }


    if (!err)
    {
        err = codec->endWriteMessage();
    }

    return err;
}

// Server shim for read_senzor_mag_accel of dac_adc interface.
erpc_status_t dac_adc_service::read_senzor_mag_accel_shim(Codec * codec, MessageBufferFactory *messageFactory, uint32_t sequence)
{
    erpc_status_t err = kErpcStatus_Success;

    Vector *results = NULL;

    // startReadMessage() was already called before this shim was invoked.

    if (!err)
    {
        err = codec->endReadMessage();
    }

    results = (Vector *) erpc_malloc(sizeof(Vector));
    if (results == NULL)
    {
        err = kErpcStatus_MemoryError;
    }

    // Invoke the actual served function.
#if ERPC_NESTED_CALLS_DETECTION
    nestingDetection = true;
#endif
    if (!err)
    {
        read_senzor_mag_accel(results);
    }
#if ERPC_NESTED_CALLS_DETECTION
    nestingDetection = false;
#endif

    // preparing MessageBuffer for serializing data
    if (!err)
    {
        err = messageFactory->prepareServerBufferForSend(codec->getBuffer());
    }

    // preparing codec for serializing data
    codec->reset();

    // Build response message.
    if (!err)
    {
        err = codec->startWriteMessage(kReplyMessage, kdac_adc_service_id, kdac_adc_read_senzor_mag_accel_id, sequence);
    }

    if (!err)
    {
        err = write_Vector_struct(codec, results);
    }

    if (!err)
    {
        err = codec->endWriteMessage();
    }

    if (results)
    {
        erpc_free(results);
    }
    return err;
}

// Server shim for board_get_name of dac_adc interface.
erpc_status_t dac_adc_service::board_get_name_shim(Codec * codec, MessageBufferFactory *messageFactory, uint32_t sequence)
{
    erpc_status_t err = kErpcStatus_Success;

    char * name = NULL;

    // startReadMessage() was already called before this shim was invoked.

    if (!err)
    {
        err = codec->endReadMessage();
    }

    name = (char *) erpc_malloc((StringMaxSize + 1) * sizeof(char));
    if (name == NULL)
    {
        err = kErpcStatus_MemoryError;
    }
    else
    {
        name[0]='\0';
    }

    // Invoke the actual served function.
#if ERPC_NESTED_CALLS_DETECTION
    nestingDetection = true;
#endif
    if (!err)
    {
        board_get_name(name);
    }
#if ERPC_NESTED_CALLS_DETECTION
    nestingDetection = false;
#endif

    // preparing MessageBuffer for serializing data
    if (!err)
    {
        err = messageFactory->prepareServerBufferForSend(codec->getBuffer());
    }

    // preparing codec for serializing data
    codec->reset();

    // Build response message.
    if (!err)
    {
        err = codec->startWriteMessage(kReplyMessage, kdac_adc_service_id, kdac_adc_board_get_name_id, sequence);
    }

    if (!err)
    {
        err = codec->writeString(strlen(name), name);
    }

    if (!err)
    {
        err = codec->endWriteMessage();
    }

    if (name)
    {
        erpc_free(name);
    }
    return err;
}
erpc_service_t create_dac_adc_service()
{
    return new (nothrow) dac_adc_service();
}
