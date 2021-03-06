#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <signal.h>
#include <sys/param.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <stdint.h>

#include <math.h>

#include "define_J.h"
#include "HAL_pid_J.h"
#include "HAL_motor_J.h"
//#include "HAL_sensor_J.h"

void pid_init(pid_control_t *p){
    p->Kp=0;
    p->Ki=0;
    p->Kd=0;
    p->err=0;
    p->errD=0;
    p->errI=0;
    p->non_ideal_D_factor=1;
    adaptive_init(p);
}

void pid_setDfactor(pid_control_t *p, motor_t val){
    p->non_ideal_D_factor = val;
}

void pid_setErr(pid_control_t *p, sensor_t val){
    p->err = val;
}

void pid_setErrD(pid_control_t *p, sensor_t val,sensor_t dt){
    p->errD = (p->non_ideal_D_factor*val/dt+(1-p->non_ideal_D_factor)*p->errD);    //dt
}
void pid_setErrI(pid_control_t *p,motor_t val,  sensor_t dt){
    p->errI += val*dt;
}
void pid_setErr_prev(pid_control_t *p, motor_t  val){
    p->err_prev = val;
}


motor_t pid_control(pid_control_t* control){
    return ((control->Kp*control->err + control->Ki*control->errI + control->Kd*control->errD));
    //control->err_prev = control->err;
    //control->errD_prev = control->errD;
    //return MV;
}

motor_t adaptive_pid_control(pid_control_t* control){
    return control->adaptive_theta*(control->Kp*control->err + control->Ki*control->errI) + control->Kd*control->errD;
    //control->err_prev = control->err;
    //control->errD_prev = control->errD;
    //return MV;
}

//(6*85^(1/2)*exp(-20*t)*sinh(2*85^(1/2)*t))/17

/*
motor_t ref_model_Z[REF_COUNT]={
    0   ,
    0.1182131372    ,
    0.232904599 ,
    0.3441513499    ,
    0.4520288945    ,
    0.5566113021    ,
    0.6579712323    ,
    0.756179959 ,
    0.8513073948    ,
    0.9434221146    ,
    1.0325913787    ,
    1.1188811562    ,
    1.2023561471    ,
    1.283079805 ,
    1.3611143584    ,
    1.4365208325    ,
    1.5093590705    ,
    1.5796877539    ,
    1.6475644235    ,
    1.713045499 ,
    1.7761862992    ,
    1.8370410612    ,
    1.8956629598    ,
    1.9521041259    ,
    2.0064156655    ,
    2.0586476777    ,
    2.1088492725    ,
    2.1570685886    ,
    2.2033528106    ,
    2.2477481863    ,
    2.2903000429    ,
    2.3310528042    ,
    2.3700500062    ,
    2.4073343135    ,
    2.4429475344    ,
    2.4769306371    ,
    2.5093237641    ,
    2.5401662476    ,
    2.5694966238    ,
    2.5973526479    ,
    2.6237713072    ,
    2.6487888364    ,
    2.6724407299    ,
    2.6947617566    ,
    2.715785972 ,
    2.7355467319    ,
    2.7540767051    ,
    2.7714078856    ,
    2.7875716057    ,
    2.8025985472    ,
    2.8165187544    ,
    2.8293616448    ,
    2.8411560217    ,
    2.8519300847    ,
    2.8617114414    ,
    2.8705271179    ,
    2.8784035702    ,
    2.8853666944    ,
    2.891441837 ,
    2.8966538054    ,
    2.9010268782    ,
    2.9045848145    ,
    2.9073508639    ,
    2.9093477763    ,
    2.9105978111    ,
    2.9111227462    ,
    2.9109438874    ,
    2.9100820773    ,
    2.908557704 ,
    2.9063907094    ,
    2.9036005984    ,
    2.9002064464    ,
    2.896226908 ,
    2.891680225 ,
    2.8865842338    ,
    2.880956374 ,
    2.8748136951    ,
    2.8681728648    ,
    2.8610501757    ,
    2.8534615529    ,
    2.845422561 ,
    2.8369484111    ,
    2.8280539676    ,
    2.8187537551    ,
    2.8090619645    ,
    2.7989924604    ,
    2.7885587864    ,
    2.7777741724    ,
    2.76665154  ,
    2.7552035088    ,
    2.7434424025    ,
    2.7313802547    ,
    2.7190288142    ,
    2.7063995512    ,
    2.6935036626    ,
    2.6803520773    ,
    2.6669554618    ,
    2.6533242252    ,
    2.6394685243    ,
    2.6253982692    ,
    2.6111231273    ,
    2.5966525292    ,
    2.5819956728    ,
    2.5671615281    ,
    2.552158842 ,
    2.5369961429    ,
    2.5216817447    ,
    2.5062237516    ,
    2.4906300623    ,
    2.4749083739    ,
    2.4590661866    ,
    2.4431108073    ,
    2.4270493535    ,
    2.4108887577    ,
    2.3946357708    ,
    2.3782969661    ,
    2.361878743 ,
    2.3453873303    ,
    2.3288287903    ,
    2.3122090217    ,
    2.2955337637    ,
    2.2788085986    ,
    2.2620389559    ,
    2.2452301148    ,
    2.2283872081    ,
    2.2115152247    ,
    2.194619013 ,
    2.1777032839    ,
    2.1607726135    ,
    2.1438314464    ,
    2.1268840982    ,
    2.1099347586    ,
    2.0929874936    ,
    2.0760462489    ,
    2.0591148519    ,
    2.0421970148    ,
    2.0252963366    ,
    2.0084163061    ,
    1.991560304 ,
    1.9747316055    ,
    1.9579333823    ,
    1.9411687056    ,
    1.9244405475    ,
    1.9077517837    ,
    1.8911051958    ,
    1.874503473 ,
    1.8579492145    ,
    1.8414449316    ,
    1.8249930492    ,
    1.8085959085    ,
    1.7922557684    ,
    1.7759748075    ,
    1.7597551263    ,
    1.7435987485    ,
    1.7275076229    ,
    1.7114836257    ,
    1.6955285613    ,
    1.6796441649    ,
    1.6638321033    ,
    1.6480939773    ,
    1.6324313227    ,
    1.6168456121    ,
    1.6013382565    ,
    1.5859106064    ,
    1.5705639539    ,
    1.5552995335    ,
    1.5401185239    ,
    1.5250220492    ,
    1.5100111802    ,
    1.495086936 ,
    1.4802502849    ,
    1.4655021461    ,
    1.4508433906    ,
    1.4362748423    ,
    1.4217972799    ,
    1.4074114371    ,
    1.3931180045    ,
    1.3789176305    ,
    1.364810922 ,
    1.3507984461    ,
    1.3368807309    ,
    1.3230582662    ,
    1.3093315052    ,
    1.2957008646    ,
    1.2821667266    ,
    1.2687294387    ,
    1.2553893158    ,
    1.2421466402    ,
    1.2290016628    ,
    1.215954604 ,
    1.2030056548    ,
    1.190154977 ,
    1.1774027047    ,
    1.1647489444    ,
    1.1521937765    ,
    1.1397372555    ,
    1.1273794112    ,
    1.1151202489    ,
    1.1029597506    ,
    1.0908978755    ,
    1.0789345605    ,
    1.0670697214    ,
    1.0553032529    ,
    1.0436350297    ,
    1.032064907 ,
    1.0205927209    ,
    1.0092182893    ,
    0.9979414126    ,
    0.9867618736    ,
    0.9756794389    ,
    0.9646938587    ,
    0.9538048679    ,
    0.9430121864    ,
    0.9323155196    ,
    0.9217145587    ,
    0.9112089816    ,
    0.9007984533    ,
    0.8904826259    ,
    0.8802611395    ,
    0.8701336227    ,
    0.8600996927    ,
    0.8501589559    ,
    0.8403110083    ,
    0.830555436 ,
    0.8208918154    ,
    0.8113197137    ,
    0.8018386891    ,
    0.7924482917    ,
    0.7831480631    ,
    0.7739375374    ,
    0.7648162409    ,
    0.7557836933    ,
    0.7468394071    ,
    0.7379828885    ,
    0.7292136375    ,
    0.7205311484    ,
    0.7119349097    ,
    0.7034244048    ,
    0.6949991119    ,
    0.6866585048    ,
    0.6784020525    ,
    0.6702292199    ,
    0.6621394681    ,
    0.6541322543    ,
    0.6462070323    ,
    0.6383632526    ,
    0.6306003628    ,
    0.6229178076    ,
    0.6153150291    ,
    0.6077914671    ,
    0.6003465592    ,
    0.5929797411    ,
    0.5856904465    ,
    0.5784781078    ,
    0.5713421558    ,
    0.56428202  ,
    0.557297129 ,
    0.5503869104    ,
    0.5435507911    ,
    0.5367881973    ,
    0.5300985551    ,
    0.52348129  ,
    0.5169358273    ,
    0.5104615927    ,
    0.5040580117    ,
    0.4977245101    ,
    0.4914605144    ,
    0.4852654511    ,
    0.4791387479    ,
    0.4730798328    ,
    0.467088135 ,
    0.4611630846    ,
    0.4553041125    ,
    0.4495106513    ,
    0.4437821345    ,
    0.438117997 ,
    0.4325176755    ,
    0.4269806079    ,
    0.4215062339    ,
    0.4160939949    ,
    0.4107433343    ,
    0.4054536971    ,
    0.4002245306    ,
    0.3950552837    ,
    0.3899454079    ,
    0.3848943565    ,
    0.3799015853    ,
    0.3749665523    ,
    0.3700887179    ,
    0.3652675448    ,
    0.3605024983    ,
    0.3557930462    ,
    0.351138659 ,
    0.3465388097    ,
    0.3419929739    ,
    0.3375006301    ,
    0.3330612596    ,
    0.3286743463    ,
    0.324339377 ,
    0.3200558416    ,
    0.3158232326    ,
    0.3116410458    ,
    0.3075087796    ,
    0.3034259357    ,
    0.2993920189    ,
    0.2954065367    ,
    0.2914690001    ,
    0.2875789229    ,
    0.2837358223    ,
    0.2799392186    ,
    0.2761886352    ,
    0.2724835987    ,
    0.268823639 ,
    0.2652082892    ,
    0.2616370857    ,
    0.2581095681    ,
    0.2546252793    ,
    0.2511837654    ,
    0.2477845761    ,
    0.244427264 ,
    0.2411113853    ,
    0.2378364994    ,
    0.2346021691    ,
    0.2314079605    ,
    0.228253443 ,
    0.2251381894    ,
    0.2220617758    ,
    0.2190237817    ,
    0.2160237898    ,
    0.2130613864    ,
    0.2101361609    ,
    0.2072477062    ,
    0.2043956185    ,
    0.2015794972    ,
    0.1987989453    ,
    0.1960535688    ,
    0.1933429774    ,
    0.1906667837    ,
    0.188024604 ,
    0.1854160576    ,
    0.1828407674    ,
    0.1802983592    ,
    0.1777884625    ,
    0.1753107097    ,
    0.1728647366    ,
    0.1704501824    ,
    0.1680666894    ,
    0.1657139031    ,
    0.1633914722    ,
    0.1610990487    ,
    0.1588362877    ,
    0.1566028476    ,
    0.1543983899    ,
    0.1522225791    ,
    0.1500750832    ,
    0.147955573 ,
    0.1458637224    ,
    0.1437992087    ,
    0.141761712 ,
    0.1397509156    ,
    0.1377665057    ,
    0.1358081717    ,
    0.133875606 ,
    0.1319685039    ,
    0.1300865637    ,
    0.1282294868    ,
    0.1263969774    ,
    0.1245887426    ,
    0.1228044926    ,
    0.1210439404    ,
    0.1193068018    ,
    0.1175927956    ,
    0.1159016434    ,
    0.1142330695    ,
    0.1125868011    ,
    0.1109625684    ,
    0.109360104 ,
    0.1077791435    ,
    0.106219425 ,
    0.1046806897    ,
    0.1031626811    ,
    0.1016651455    ,
    0.1001878321    ,
    0.0987304923    ,
    0.0972928805    ,
    0.0958747535    ,
    0.0944758707    ,
    0.0930959941    ,
    0.0917348883    ,
    0.0903923202    ,
    0.0890680595    ,
    0.0877618782    ,
    0.0864735507    ,
    0.085202854 ,
    0.0839495675    ,
    0.0827134729    ,
    0.0814943544    ,
    0.0802919985    ,
    0.0791061941    ,
    0.0779367323    ,
    0.0767834067    ,
    0.0756460131    ,
    0.0745243494    ,
    0.073418216 ,
    0.0723274155    ,
    0.0712517526    ,
    0.0701910342    ,
    0.0691450694    ,
    0.0681136695    ,
    0.0670966479    ,
    0.0660938201    ,
    0.0651050037    ,
    0.0641300185    ,
    0.063168686 ,
    0.0622208302    ,
    0.0612862768    ,
    0.0603648537    ,
    0.0594563907    ,
    0.0585607194    ,
    0.0576776736    ,
    0.056807089 ,
    0.0559488032    ,
    0.0551026554    ,
    0.0542684872    ,
    0.0534461415    ,
    0.0526354636    ,
    0.0518363001    ,
    0.0510484998    ,
    0.050271913 ,
    0.0495063921    ,
    0.0487517908    ,
    0.0480079649    ,
    0.0472747719    ,
    0.0465520707    ,
    0.0458397223    ,
    0.045137589 ,
    0.0444455349    ,
    0.0437634258    ,
    0.0430911291    ,
    0.0424285137    ,
    0.0417754501    ,
    0.0411318104    ,
    0.0404974683    ,
    0.039872299 ,
    0.0392561792    ,
    0.0386489871    ,
    0.0380506024    ,
    0.0374609064    ,
    0.0368797815    ,
    0.036307112 ,
    0.0357427833    ,
    0.0351866823    ,
    0.0346386974    ,
    0.0340987182    ,
    0.0335666358    ,
    0.0330423426    ,
    0.0325257324    ,
    0.0320167003    ,
    0.0315151426    ,
    0.0310209569    ,
    0.0305340424    ,
    0.0300542992    ,
    0.0295816287    ,
    0.0291159338    ,
    0.0286571183    ,
    0.0282050875    ,
    0.0277597476    ,
    0.0273210062    ,
    0.0268887721    ,
    0.0264629551    ,
    0.0260434662    ,
    0.0256302176    ,
    0.0252231225    ,
    0.0248220954    ,
    0.0244270518    ,
    0.0240379081    ,
    0.0236545821    ,
    0.0232769923    ,
    0.0229050587    ,
    0.0225387018    ,
    0.0221778436    ,
    0.0218224069    ,
    0.0214723154    ,
    0.0211274939    ,
    0.0207878683    ,
    0.0204533652    ,
    0.0201239124    ,
    0.0197994384    ,
    0.0194798729    ,
    0.0191651464    ,
    0.0188551902    ,
    0.0185499366    ,
    0.0182493189    ,
    0.017953271 ,
    0.017661728 ,
    0.0173746256    ,
    0.0170919004    ,
    0.01681349  ,
    0.0165393327    ,
    0.0162693675    
};

*/

#if 0
void adaptive_getRefModel(){

    int i;
    float_j_t t=0;
    for(i=0;i<REF_COUNT ;i++){
        //        ref_model_Z[i]= (6*powf(85,(1/2))*exp(-20*t)*sinh(2*powf(85,(1/2))*t))/17;
        ref_model_Z[i] = 8*15*powf(15,(1/2))*exp(-(15*t)/2)*sin((powf(15,(1/2))*t)/2);
        t += INIT_DT;
    }

}

#endif 


/*
 * adaptive control part
 */

void adaptive_init(pid_control_t* p){
    p->gamma_factor =0.01;
    p->adaptive_theta=1;
#if 0
    int i;
    for(i=0;i<REF_COUNT;i++){
        p->input_model_Z[i]=0;
    }

#endif 

}

void adaptive_setGamma(pid_control_t*p,motor_t val){
    p->gamma_factor = val;
}



int adaptive_ref_sys_model(pid_control_t* p ,motor_t input,motor_t dt){
#if 0
    int i;
    memmove(&p->input_model_Z[1], &p->input_model_Z[0], sizeof(p->input_model_Z) - sizeof(*p->input_model_Z));
    p->input_model_Z[0] = input;
    motor_t current_state = 0;
    for(i=0;i<REF_COUNT;i++){
        current_state += p->input_model_Z[i]*ref_model_Z[i];
    }
    return current_state*dt;
#else
static float_j_t ss_A[2][2]={0.970327909120375 , -0.118213137219007, 0.00197021895365012, 0.999881193425127 };    
static float_j_t ss_B[2]={ 0.00197021895365012 , 1.98010958121718 * powf(10, -6)};    
static float_j_t ss_C[2]={0,60};    

p->x1_k=ss_A[0][0]*p->x1_k+ss_A[0][1]*p->x2_k+ss_B[0]*input;
p->x2_k=ss_A[1][0]*p->x1_k+ss_A[1][1]*p->x2_k+ss_B[1]*input;
p->state = ss_C[1]*p->x2_k;


return 0;

#endif



}

void adaptive_setFuzzy_factor(pid_control_t* p,float_j_t Ye, float_j_t dYe_dt){
    p->Ye_div_factor=Ye;
    p->dYe_dt_div_factor = dYe_dt;
}


void adaptive_setFuzzy_table(pid_control_t* p, int matrix[FUZZY_TABLE][FUZZY_TABLE]){
    int i,j;
    for(i=0;i<FUZZY_TABLE;i++){
        for(j=0;j<FUZZY_TABLE;j++){
            p->fuzzy_table[i][j]=matrix[i][j];
            printf("%d ",p->fuzzy_table[i][j]);
        }
        printf("\n");
    }
}

void adaptive_setFuzzy_ref_table(pid_control_t* p , int matrix[FUZZY_TABLE]){
    int i;
    for(i=0;i<FUZZY_TABLE;i++){
        p->fuzzy_ref_table[i]=matrix[i];
    }
}

float_j_t fuzzy_membership_function(pid_control_t* pid, float_j_t x, int desire){
    float_j_t r,p,q;

    if(desire<0){
        r = pid->fuzzy_ref_table[0];
        p = 1000000000;
        q = pid->fuzzy_ref_table[1];
    }else if(desire>FUZZY_TABLE-1){
        r = pid->fuzzy_ref_table[FUZZY_TABLE-1];
        p = pid->fuzzy_ref_table[FUZZY_TABLE-2];
        q = 1000000000;
    }else{
        r = pid->fuzzy_ref_table[desire];
        p = pid->fuzzy_ref_table[desire-1];
        q = pid->fuzzy_ref_table[desire+1];
    }

    //printf("r %f q %f p %f \n",r,q,p);

    float_j_t u_f;
    if((p<=x)&&(x<=r)){
        u_f = 1/(r-p)*(x-r)+1;
    }else if((r<=x)&&(x<=q)){
        u_f = -1/(q-r)*(x-r)+1;
    }else{
        u_f = 0;
    }

    //printf("u_f %f\n",u_f);
    return u_f;
}

void print_table(pid_control_t* p){
    int i,j;
    for(i=0;i<FUZZY_TABLE;i++){
        for(j=0;j<FUZZY_TABLE;j++){
            printf("%d ",p->fuzzy_table[i][j]);
        }
        printf("\n");
    }
}



int adaptive_theta_calc(pid_control_t* p , motor_t plant_feedback,float_j_t dt){
#define YE_LPF_FACTOR 0.4f
#define DYE_DT_LPF_FACTOR 0.2f
    float_j_t ref = p->state;


    float_j_t Ye = (ref-plant_feedback)/p->Ye_div_factor;
    float_j_t dYe_dt = ( (Ye - p->Ye_prev)/p->dYe_dt_div_factor /dt)*DYE_DT_LPF_FACTOR + (1-DYE_DT_LPF_FACTOR)*p->dYe_dt_prev  ;
    p->Ye_prev = Ye;
    p->dYe_dt_prev = dYe_dt;




//    printf("ye %f dye %f \n",Ye,dYe_dt);

    //printf("input Ye  dYe \n");
    //scanf("%f %f",&Ye,&dYe_dt);
    //print_table(p);
    //Ye+=(FUZZY_TABLE-1)/2;
    //dYe_dt+=(FUZZY_TABLE-1)/2;

#if 1

#define  offset  (FUZZY_TABLE-1)/2
    Ye += offset;
    dYe_dt += offset;

    if(Ye>FUZZY_TABLE-1)
        Ye = FUZZY_TABLE-1;
    if(Ye<0)
        Ye = 0;
    if(dYe_dt>FUZZY_TABLE-1)
        dYe_dt =FUZZY_TABLE-1;
    if(dYe_dt<0)
        dYe_dt=0;


    float_j_t a=0,b=0,c=0,d=0,delta_theta=0;
    a = p->fuzzy_table[(int)floor(dYe_dt)][(int)floor(Ye)] * fuzzy_membership_function(p,Ye-offset,(int)floor(Ye));
    //printf("a table %d mem %f \n",p->fuzzy_table[(int)floor(dYe_dt)][(int)floor(Ye)], fuzzy_membership_function(p,Ye-offset,(int)floor(Ye)));

    if(floor(Ye)!=ceil(Ye)){
        b = p->fuzzy_table[(int)floor(dYe_dt)][(int)ceil(Ye)] * fuzzy_membership_function(p,Ye-offset,(int)ceil(Ye));
        //printf("b table %d mem %f \n",p->fuzzy_table[(int)floor(dYe_dt)][(int)ceil(Ye)], fuzzy_membership_function(p,Ye-offset,(int)ceil(Ye)));
    }

    c = p->fuzzy_table[(int)ceil(dYe_dt)][(int)floor(Ye)] * fuzzy_membership_function(p,Ye-offset,(int)floor(Ye));
    //printf("c table %d mem %f \n",p->fuzzy_table[(int)ceil(dYe_dt)][(int)floor(Ye)], fuzzy_membership_function(p,Ye-offset,(int)floor(Ye)));


    if(floor(Ye)!=ceil(Ye)){
        d = p->fuzzy_table[(int)ceil(dYe_dt)][(int)ceil(Ye)] * fuzzy_membership_function(p,Ye-offset,(int)ceil(Ye));
        //printf("d table %d mem %f \n",p->fuzzy_table[(int)ceil(dYe_dt)][(int)ceil(Ye)], fuzzy_membership_function(p,Ye-offset,(int)ceil(Ye)));
    }
    delta_theta= p->gamma_factor*((a+b)*fuzzy_membership_function(p,dYe_dt-offset,floor(dYe_dt)) );
    if(floor(dYe_dt)!=ceil(dYe_dt)){
        delta_theta += p->gamma_factor*((c+d)*fuzzy_membership_function(p,dYe_dt-offset,ceil(dYe_dt)));
    }
    //printf("a %f b %f c %f d %f \n",a,b,c,d);

    //printf("out Ye %f dYe_dt %f delta_theta %f \n",Ye-offset,dYe_dt-offset,delta_theta);
    //    printf("del %f ",delta_theta);
    p->adaptive_theta = delta_theta+1;
    //p->adaptive_theta += delta_theta;

    /*
       if((plant_feedback-ref)>0){   
       if(p->adaptive_theta>0)
       p->adaptive_theta += (plant_feedback-ref)*ref*(-p->gamma_factor);
       else
       p->adaptive_theta -=(plant_feedback-ref)*ref*(-p->gamma_factor);
       }else{
       if(p->adaptive_theta>0)
       p->adaptive_theta += (plant_feedback-ref)*ref*(-p->gamma_factor);
       else
       p->adaptive_theta -=(plant_feedback-ref)*ref*(-p->gamma_factor);
       }
       */  



    /*    if(p->adaptive_theta>10){
          p->adaptive_theta = 10;
          }else if(p->adaptive_theta<0.1){
          p->adaptive_theta = 0.1;
          }*/

    return 0;
#else
    float_j_t Ye = plant_feedback-ref;
    float_j_t dYe_dt = Ye - prev_Ye;
    p->prev_Ye = Ye;

    p->adaptive_theta += abs(Ye)*(p->gamma_factor);

    if(p->adaptive_theta<0){
        p->adaptive_theta = -p->adaptive_theta;
    }

    if(p->adaptive_theta>5){
        p->adaptive_theta = 5;
    }
    return p->adaptive_theta;
#endif
}



motor_t adaptive_control(motor_t pid_out, motor_t adaptive_theta){
    return pid_out * (adaptive_theta);
}


/*
   int TF_function(TF_t t ,motor_t val,sensor_t dt){
   motor_t err;
   int MV;
//motor_t delta_mv;
motor_t errD;
//motor_t MV;
err = val-target[t];
//mconst[t][errI] += err*dt;
//PID[t].errI += err*dt;

//errD = (WEIGHT*(err-mconst[t][err_prev])/dt+(1-WEIGHT)*(mconst[t][errD_prev]));

if(t == ROLL){
errD = gyroX*250/32768/dt;
}else{
errD = gyroY*250/32768/dt;
}

//errD = (err-mconst[t][err_prev])/dt;
//MV = (int)(mconst[t][Kp]*err+ mconst[t][Ki]*mconst[t][errI] + mconst[t][Kd]*errD);
//MV = (int)(PID[t].Kp*err + PID[t].Ki*PID[t].errI + PID[t].Kd*errD);
if(MV>=10000){
MV=10000;
//PID[t].errI -= err*dt;
//mconst[t][errI] -= err*dt;
//	mconst[t][errI] -=err;
}else if(MV<=-10000){
MV=-10000;
//PID[t].errI -= err*dt;
//mconst[t][errI] -= err*dt;
//	mconst[t][errI] -=err;
}
PID[t].err_prev = err;
PID[t].errD_prev = errD;
//mconst[t][err_prev]=err;
//mconst[t][errD_prev]=errD;
return (int)MV;
}*/

uint32_t pid_feedforward(int x){

    if(x<0)return 0;
    if(x>10000)return 10000;

    //9.6618e-09 x^3 - 6.3001e-05 x^2 + 0.67372 x - 10.634
    motor_t x2 = x*x;
    motor_t x3 = x2*x;

    motor_t y = 0.0000000096618*x3+0.000063001*x2+0.67372*x -10.634;

#ifdef __FF__

static float_j_t ff_A[2][2]={0.970327909120375 , -0.118213137219007, 0.00197021895365012, 0.999881193425127 };    
static float_j_t ff_B[2]={ 0.00197021895365012 , 1.98010958121718 * powf(10, -6)};    
static float_j_t ff_C[2]={0,60};    
static float_j_t state_x1,state_x2;

state_x1=ff_A[0][0]*state_x1+ff_A[0][1]*state_x2+ff_B[0]*x;
state_x2=ff_A[1][0]*state_x1+ff_A[1][1]*state_x2+ff_B[1]*x;
return (y = ff_C[1]*state_x2);

#endif 

    /*if(y>10000){
        return 10000;
    }else if(y<0){
        return 0;
    }else{
        return (uint32_t)y;
    }*/
        return (uint32_t)y;
    //
    ////motor_t x = ((motor_t)m_val)*0.065725; 
    //	motor_t x1 = m_val*328.625/10000;		
    ////	motor_t x1 = m_val*657.25/10000;
    //	motor_t x2 = x1*x1;
    //	motor_t x3 = x2*x1;
    //	motor_t x4 = x3*x1;
    //	motor_t	x5 = x4*x1;
    //
    //	//motor_t y = 4.4887*powf(10,-10)*x5 - 5.874*powf(10,-7)* x4 + 0.00030083*x3 - 0.057589 *x2 + 6.2994*x1 - 0.098165;
    //	motor_t y = 0.000000014364 * x5 - 0.0000093985 * x4 + 0.0024067* x3 - 0.23035*x2 + 12.5988*x1 - 0.098165;
    ////	motor_t y = 0.00000000044887f*x5 - 0.0000005874f*x4 + 0.00030083f*x3 - 0.057589f *x2 + 6.2994f*x1 - 0.098165f;
    //	return (uint32_t)y;
}



