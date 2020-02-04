// Save as "HelloJNI.c"

/*
 * Class:     NBioBSPJNI
 * Method:    JNI
 * Author:    Lincoln Berlick
 *
 */


#include <iostream>
#include <string>
#include <jni.h>
#include "NBioBSPJNI.h"   // Generated
#include "NBioAPI.h"
#include "NBioAPI_IndexSearch.h"
#include <cstring>

using namespace std;



NBioAPI_HANDLE m_hNBioBSP;


bool isOpen = false;


    void IniciarSDK(){

        if(!isOpen){
            NBioAPI_Init(&m_hNBioBSP);
            cout << "open" << endl;
            isOpen = true;
        }

    }

NBioAPI_RETURN MyCaptureCallback(NBioAPI_WINDOW_CALLBACK_PARAM_PTR_0 pCallbackParam, NBioAPI_VOID_PTR pUserParam)
{

    return NBioAPIERROR_NONE;
}

long digitos;


NBioAPI_INPUT_FIR pegarinputfitdefirhandle(JNIEnv *env, jobject INPUT_FIR){

    NBioAPI_RETURN nRet;
    NBioAPI_INPUT_FIR inputFIR;

    //recebe objecto de java e adiciona a objeto nativo
    jclass thisClass = env->GetObjectClass(INPUT_FIR);
    jfieldID field_FIRHandle = env->GetFieldID(thisClass, "FIRHandle", "J");
    jfieldID field_Form = env->GetFieldID(thisClass, "Form", "I");

    digitos = env->GetLongField(INPUT_FIR, field_FIRHandle);
    int form = env->GetIntField(INPUT_FIR, field_Form);

    inputFIR.InputFIR.FIR = &digitos;
    inputFIR.Form = form;
    //long value = inputFIR.InputFIR.FIR;

    return  inputFIR;
}


NBioAPI_FIR_HANDLE getFIR_HANDLE(JNIEnv *env, jobject fir_handle){

    NBioAPI_FIR_HANDLE firhandle; // retorno

    jclass thisclass = env->GetObjectClass(fir_handle);

    jfieldID field_inputfir =  env->GetFieldID(thisclass, "Handle", "I");
    long digital = env->GetLongField(fir_handle, field_inputfir);

    firhandle = digital;

    return firhandle;



}



int destroyobject(NBioAPI_HANDLE handle, NBioAPI_FIR_HANDLE *firhandle){

    return NBioAPI_FreeFIRHandle(handle, *firhandle);

}



JNIEXPORT NBioAPI_RETURN JNICALL Java_NBioBSPJNI_NBioBSPJNI_NativeOpenDevice(JNIEnv *env, jobject thisObj) {

    IniciarSDK();

    NBioAPI_RETURN nRet;
    NBioAPI_DEVICE_ID deviceID = NBioAPI_GetOpenedDeviceID(m_hNBioBSP); //pega o id do dispositivo

    if(NBioAPI_DEVICE_ID_NONE != deviceID){
        NBioAPI_CloseDevice(m_hNBioBSP,deviceID);
    }

    nRet = NBioAPI_OpenDevice(m_hNBioBSP,NBioAPI_DEVICE_ID_AUTO);
    if(NBioAPIERROR_NONE == nRet){
        //nRet = NBioAPI_GetDeviceInfo(m_hNBioBSP, deviceID, 0, &m_hNBioBSP);
    }
    return nRet;
}

JNIEXPORT NBioAPI_RETURN JNICALL Java_NBioBSPJNI_NBioBSPJNI_NativeCloseDevice(JNIEnv *env, jobject thisObj) {

    IniciarSDK();

    NBioAPI_RETURN nRet;
   //pega o id do dispositivo
    NBioAPI_DEVICE_ID deviceID = NBioAPI_GetOpenedDeviceID(m_hNBioBSP);
        //Device Close.
    nRet = NBioAPI_CloseDevice(m_hNBioBSP, deviceID);
       //NBioAPI Terminate
    NBioAPI_Terminate(m_hNBioBSP);

    return nRet;
}

JNIEXPORT void JNICALL Java_NBioBSPJNI_NBioBSPJNI_TesteObject
  (JNIEnv *env, jobject thisObj, jobject  b) {
    jfieldID fid; /* store the field ID */
   jclass c = env->GetObjectClass(b);

   fid = env->GetFieldID(c,"value", "Z");
   jbyte old = env->GetBooleanField(b,fid);
   cout << "capturado " << old << endl;

   env->SetBooleanField(b,fid,0);

   return ;

  }


  JNIEXPORT NBioAPI_RETURN JNICALL Java_NBioBSPJNI_NBioBSPJNI_NativeVerifyMatch
  (JNIEnv *env, jobject thisObj, jobject  FIR_HANDLE1, jobject FIRHANDLE2, jobject booleano, jobject PAY_LOAD) {

    NBioAPI_RETURN nRet;

    NBioAPI_INPUT_FIR inputfir1;
    NBioAPI_INPUT_FIR inputfir2;

    NBioAPI_BOOL resultado;

    NBioAPI_FIR_PAYLOAD payload;


    inputfir1 = pegarinputfitdefirhandle(env, FIR_HANDLE1);
    inputfir2 = pegarinputfitdefirhandle(env, FIRHANDLE2);

    nRet = NBioAPI_VerifyMatch(m_hNBioBSP, &inputfir1, &inputfir2,&resultado,&payload);

    //cout << "capturado " << old << endl;

    jfieldID fid; /* store the field ID */
    jclass c = env->GetObjectClass(booleano);
    fid = env->GetFieldID(c,"value", "Z");
    //jbyte old = env->GetBooleanField(paramBoolean,fid);
    env->SetBooleanField(booleano,fid, resultado);

  //  env->SetBooleanField(booleano,fid,0);

    return nRet;
  }




  NBioAPI_FIR_PURPOSE verificarpurpose(jint proposito)
  {

      if(proposito == 0) {

        return NBioAPI_FIR_PURPOSE_IDENTIFY;
      }

      NBioAPI_FIR_PURPOSE purpose;
      switch(proposito){
        case 1:
            purpose = NBioAPI_FIR_PURPOSE_VERIFY;
            break;

        case 2:
            purpose = NBioAPI_FIR_PURPOSE_IDENTIFY;
            break;

        case 3:
            purpose = NBioAPI_FIR_PURPOSE_ENROLL;
            break;

        case 4:
            purpose = NBioAPI_FIR_PURPOSE_ENROLL_FOR_VERIFICATION_ONLY;
            break;

        case 5:
            purpose = NBioAPI_FIR_PURPOSE_ENROLL_FOR_IDENTIFICATION_ONLY;
            break;

        case 6:
            purpose = NBioAPI_FIR_PURPOSE_AUDIT;
            break;

        case 16:
            purpose = NBioAPI_FIR_PURPOSE_UPDATE;
            break;
      }

    return purpose;


  }



  //winoption é irrelevante para linux nbiobsp nativo não chama interface gráfica no linux.
  JNIEXPORT void JNICALL Java_NBioBSPJNI_NBioBSPJNI_NativeCapture
  (JNIEnv *env,jobject thisObj, jint proposito,jobject fir_handle, jint timeout, jobject fir_handleaudit, jobject winoption)  {


    NBioAPI_WINDOW_OPTION winOption;
    NBioAPI_FIR_HANDLE hCapturedFIR1;
    NBioAPI_RETURN nRet;

    int timeoutnativo = ((int) timeout <= 0) ? -1 : timeout;

    nRet = NBioAPI_Capture(m_hNBioBSP, verificarpurpose(proposito), &hCapturedFIR1, timeoutnativo, NULL, &winOption);
    jclass thisClass = env->GetObjectClass(fir_handle);
    jfieldID field = env->GetFieldID(thisClass, "Handle", "J");
    //long number = env->GetLongField(fir_handle, field);
    env->SetLongField(fir_handle, field, hCapturedFIR1);


    //NBioAPI_FreeFIRHandle(m_hNBioBSP, hCapturedFIR1);

    return;

  }





void getINPUT_FIR(JNIEnv *env, jobject input_fir, NBioAPI_INPUT_FIR &fir, long &id){

    jclass thisClass = env->FindClass("NBioBSPJNI/NBioBSPJNI$INPUT_FIR");
    jfieldID field_FIRHandle = env->GetFieldID(thisClass, "FIRHandle", "J");
    jfieldID field_Form = env->GetFieldID(thisClass, "Form", "I");

    return;

    }





JNIEXPORT NBioAPI_RETURN JNICALL Java_NBioBSPJNI_NBioBSPJNI_NativeVerify  (JNIEnv *env,jobject thisObj, jobject INPUT_FIR, jobject paramBoolean, jobject PAY_LOAD) {

    //IniciarSDK();
    NBioAPI_RETURN nRet;
    NBioAPI_FIR_PAYLOAD payload;        // Windows Option setting
    NBioAPI_WINDOW_OPTION winOption;
    NBioAPI_BOOL resultado;


    //recebe objecto de java e adiciona a objeto nativo
    jclass thisClass = env->GetObjectClass(INPUT_FIR);
    jfieldID field_FIRHandle = env->GetFieldID(thisClass, "FIRHandle", "J");

    jfieldID field_Form = env->GetFieldID(thisClass, "Form", "I");

    long digital = env->GetLongField(INPUT_FIR, field_FIRHandle);
    int form = env->GetIntField(INPUT_FIR, field_Form);

    NBioAPI_INPUT_FIR inputFIR = pegarinputfitdefirhandle(env,INPUT_FIR);

    //cout << "capturado verify digital  " << digital << endl;



    nRet = NBioAPI_Verify(m_hNBioBSP, &inputFIR, &resultado, &payload, -1, NULL, &winOption);
    // fim objeto nativo

    //devolve valor booleano
    jfieldID fid; /* store the field ID */
    jclass c = env->GetObjectClass(paramBoolean);
    fid = env->GetFieldID(c,"value", "Z");
    //jbyte old = env->GetBooleanField(paramBoolean,fid);4
    env->SetBooleanField(paramBoolean,fid, resultado);

    return nRet;

     }



     //INDEX SEARCH

 JNIEXPORT NBioAPI_RETURN JNICALL Java_NBioBSPJNI_NBioBSPJNI_NativeInitIndexsearch  (JNIEnv *env,jobject thisObj) {


    NBioAPI_RETURN nRet = NBioAPI_InitIndexSearchEngine(m_hNBioBSP);
    return nRet;



 }









