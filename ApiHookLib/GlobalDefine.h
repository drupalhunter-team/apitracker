#pragma once

#define RETURN_IF(x)							do{if(x) return;}while(false);
#define RETURN_VALUE_IF(x, v)					do{if(x) return v;}while(false);

#define RETURN_IF_NULL(x)						do{if(x == NULL) return;}while(false);
#define RETURN_VALUE_IF_NULL(x, v)				do{if(x == NULL) return v;}while(false);