void main()
{
        vec3 paramDiffuse;
        vec3 paramEmissive;
        vec3 paramNormals;
        float paramSpecular;
        float paramSpecularPower;

        computeShaderParameters(paramDiffuse, paramNormals, paramSpecular, paramSpecularPower, paramEmissive);

        outColor = vec4( paramDiffuse, paramSpecular );
        outHighValues = vec4(paramEmissive, 1.0);

        vec3 bumpNormal = paramNormals;
        bumpNormal = normalize(bumpNormal * 2.0 - 1.0);
        bumpNormal = normalize(In.TBN * bumpNormal);
        outNormal = vec4( bumpNormal*0.5+0.5, paramSpecularPower/100.0 );

        outPositions = vec4(In.Position, 1.0);
}
